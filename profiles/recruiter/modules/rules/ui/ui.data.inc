<?php

/**
 * @file Contains data type related forms.
 */


/**
 * Interface for data types providing a direct input form.
 */
interface RulesDataDirectInputFormInterface {

  /**
   * Constructs the direct input form.
   *
   * @return Array
   *  The direct input form.
   */
  public static function inputForm($name, $info, $settings, RulesPlugin $element);

  /**
   * Render the configured value.
   *
   * @return Array
   *   A renderable array.
   */
  public static function render($value);

}

/**
 * Default UI related class for data types.
 */
class RulesDataUI {

  /**
   * Specifies the default input mode per data type.
   */
  public static function getDefaultMode() {
    return 'selector';
  }

  /**
   * Provides the selection form for a parameter.
   */
  public static function selectionForm($name, $info, $settings, RulesPlugin $element) {
    if (!isset($settings[$name . ':select'])) {
      $settings[$name . ':select'] = '';
      $vars = $element->availableVariables();
      // Default to variables with the same name as the parameter.
      if (isset($vars[$name])) {
        $settings[$name . ':select'] = $name;
      }
      // If there is only one match, use it by default.
      elseif (count($matches = RulesData::matchingDataSelector($vars, $info, '', 1, FALSE)) == 1) {
        $settings[$name . ':select'] = key($matches);
      }
    }
    $form[$name . ':select'] = array(
      '#type' => 'rules_data_selection',
      '#title' => t('Data selector'),
      '#default_value' => $settings[$name . ':select'],
      '#required' => empty($info['optional']),
      '#autocomplete_path' => RulesPluginUI::path($element->root()->name, 'autocomplete' . '/' . $name),
      // Make the autocomplete textfield big enough so that it can display
      // descriptions without word wraps.
      '#size' => 75,
    );
    $form['help'] = array(
      '#theme' => 'rules_data_selector_help',
      '#variables' => $element->availableVariables(),
      '#parameter' => $info,
    );

    // Add data processor.
    $settings += array($name . ':process' => array());
    $form[$name . ':process'] = array();
    RulesDataProcessor::attachForm($form[$name . ':process'], $settings[$name . ':process'], $info, $element->availableVariables());
    return $form;
  }

  /**
   * Renders the value by making use of the label if an options list is available.
   *
   * Used for data UI classes implementing the
   * RulesDataDirectInputFormInterface.
   *
   * In case an options list is available, the the usual render() method won't
   * be invoked, instead the selected entry is rendered via this method.
   *
   * @todo for Drupal 8: Refactor to avoid implementations have to care about
   * option lists when generating the form, but not when rendering values.
   */
  public static function renderOptionsLabel($value, $name, $info, RulesPlugin $element) {
    if (!empty($info['options list'])) {
      $element->call('loadBasicInclude');
      $options = entity_property_options_flatten($info['options list']($element, $name));
      if (!is_array($value) && isset($options[$value])) {
        $value = $options[$value];
      }
      else {
        foreach ($value as $key => $single_value) {
          if (isset($options[$single_value])) {
            $value[$key] = $options[$single_value];
          }
        }
        $value = implode(', ', $value);
      }
      return array(
        'content' => array('#markup' => check_plain($value)),
        '#attributes' => array('class' => array('rules-parameter-options-entry')),
      );
    }
  }
}

/**
 * UI for textual data.
 */
class RulesDataUIText extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    if (!empty($info['options list'])) {
      // Make sure the .rules.inc of the providing module is included as the
      // options list callback may reside there.
      $element->call('loadBasicInclude');
      $form[$name] = array(
        '#type' => 'select',
        '#options' => $info['options list']($element, $name),
      );
    }
    else {
      $form[$name] = array(
        '#type' => 'textarea',
      );
      RulesDataInputEvaluator::attachForm($form, $settings, $info, $element->availableVariables());
    }
    $settings += array($name => isset($info['default value']) ? $info['default value'] : '');
    $form[$name] += array(
      '#title' => t('Value'),
      '#default_value' => $settings[$name],
      '#required' => empty($info['optional']),
    );
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => check_plain($value)),
      '#attributes' => array('class' => array('rules-parameter-text')),
    );
  }
}

/**
 * UI for text tokens.
 */
class RulesDataUITextToken extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form['#description'] = t('May only contain only lowercase letters, numbers, and underscores.');
    return $form;
  }
}

/**
 * UI for formatted text.
 */
class RulesDataUITextFormatted extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $settings += array($name => isset($info['default value']) ? $info['default value'] : array('value' => '', 'format' => NULL));

    $form[$name]['#type'] = 'text_format';
    $form[$name]['#base_type'] = 'textarea';
    $form[$name]['#default_value'] = $settings[$name]['value'];
    $form[$name]['#format'] = $settings[$name]['format'];
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => check_plain($value['value'])),
      '#attributes' => array('class' => array('rules-parameter-text-formatted')),
    );
  }
}



/**
 * UI for decimal data.
 */
class RulesDataUIDecimal extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    if (empty($info['options list'])) {
      $form[$name]['#type'] = 'textfield';
    }
    $form[$name]['#element_validate'][] = 'rules_ui_element_decimal_validate';
    return $form;
  }
}

/**
 * UI for integers.
 */
class RulesDataUIInteger extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    if (empty($info['options list'])) {
      $form[$name]['#type'] = 'textfield';
    }
    $form[$name]['#element_validate'][] = 'rules_ui_element_integer_validate';
    return $form;
  }
}

/**
 * UI for boolean data.
 */
class RulesDataUIBoolean extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => isset($info['default value']) ? $info['default value'] : '');
    $form[$name] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($info['label']),
      '#default_value' => !empty($settings[$name]),
    );
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => !empty($value) ? t('true') : t('false')),
      '#attributes' => array('class' => array('rules-parameter-boolean')),
    );
  }
}

/**
 * UI for dates.
 */
class RulesDataUIDate extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => gmdate('Y-m-d H:i:s', time()));
    // Convert any configured timestamp into a readable format.
    if (is_numeric($settings[$name])) {
      $settings[$name] = gmdate('Y-m-d H:i:s', $settings[$name]);
    }
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'textfield';
    $form[$name]['#element_validate'][] = 'rules_ui_element_date_validate';
    // Note that the date input evaluator takes care for parsing dates using
    // strtotime() into a timestamp, which is the internal date format.
    $form[$name]['#description'] = t('The date in GMT. Format: %format or other values in GMT known by the PHP !strtotime function like "+1 day". Relative dates like "+1 day" or "now" relate to the evaluation time.', array('%format' => gmdate('Y-m-d H:i:s', time() + 86400), '!strtotime' => l('strtotime()', 'http://php.net/strtotime')));

    //TODO: Leverage the jquery datepicker+timepicker once a module providing
    //the timpeicker is available.
    return $form;
  }

  public static function render($value) {
    $value = is_numeric($value) ? format_date($value, 'short') : check_plain($value);
    return array(
      'content' => array('#markup' => $value),
      '#attributes' => array('class' => array('rules-parameter-date')),
    );
  }
}

/**
 * UI for duration type parameter.
 */
class RulesDataUIDuration extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'rules_duration';
    return $form;
  }

  public static function render($value) {
    $value = is_numeric($value) ? format_interval($value) : check_plain($value);
    return array(
      'content' => array('#markup' => $value),
      '#attributes' => array('class' => array('rules-parameter-duration')),
    );
  }
}

/**
 * UI for the URI type parameter.
 */
class RulesDataUIURI extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#rows'] = 2;
    return $form;
  }
}

/**
 * UI for lists of textual data.
 */
class RulesDataUIListText extends RulesDataUIText {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => isset($info['default value']) ? $info['default value'] : array());
    $form = parent::inputForm($name, $info, $settings, $element);
    if ($form[$name]['#type'] == 'textarea') {
      // Fix up the value to be an array during after build.
      $form[$name]['#after_build'][] = 'rules_ui_list_textarea_after_build';
      $form[$name]['#pre_render'][] = 'rules_ui_list_textarea_pre_render';
      $form[$name]['#default_value'] = implode("\n", $settings[$name]);
      $form[$name]['#description'] = t('A list of values, one on each line.');
    }
    else {
      $form[$name]['#multiple'] = TRUE;
    }
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => check_plain(implode(', ', $value))),
      '#attributes' => array('class' => array('rules-parameter-list')),
    );
  }
}

/**
 * UI for lists of integers.
 */
class RulesDataUIListInteger extends RulesDataUIListText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#element_validate'][] = 'rules_ui_element_integer_list_validate';
    return $form;
  }
}

/**
 * UI for entity-based data types.
 */
class RulesDataUIEntity extends RulesDataUIText {

  public static function getDefaultMode() {
    return 'select';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    if (empty($info['options list'])) {
      $form[$name]['#type'] = 'textfield';

      $entity_info = entity_get_info($info['type']);
      if (empty($entity_info['entity keys']['name'])) {
        $form[$name]['#element_validate'][] = 'rules_ui_element_integer_validate';
      }
      $form[$name]['#title'] = t('@entity identifier', array('@entity' => $entity_info['label']));
      $entity_label = strtolower($entity_info['label'][0]) . substr($entity_info['label'], 1);
      $form[$name]['#description'] = t('Specify the identfier of the @entity that should be used.', array('@entity' => $entity_label));
    }
    return $form;
  }
}

/**
 * UI for exportable entity-based data types.
 */
class RulesDataUIEntityExportable extends RulesDataUIEntity {

  public static function getDefaultMode() {
    return 'input';
  }
}
