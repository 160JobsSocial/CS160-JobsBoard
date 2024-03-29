<?php

/**
 * @file
 * Display plugin for displaying the search facets in a block.
 */

/**
 * Plugin class for displaying search facets in a block.
 */
class SearchApiViewsFacetsBlockDisplay extends views_plugin_display_block {
  public function displays_exposed() {
    return FALSE;
  }
  public function uses_exposed() {
    return FALSE;
  }

  public function option_definition() {
    $options = parent::option_definition();

    $options['linked_path'] = array('default' => '');
    $options['facet_field'] = '';

    return $options;
  }

  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    if (substr($this->view->base_table, 0, 17) != 'search_api_index_') {
      return;
    }

    switch ($form_state['section']) {
      case 'linked_path':
        $form['#title'] .= t('Search page path');
        $form['linked_path'] = array(
          '#type' => 'textfield',
          '#description' => t('The menu path to which search facets will link. Leave empty to use the current path.'),
          '#default_value' => $this->get_option('linked_path'),
        );
        break;
      case 'facet_field':
        $form['facet_field'] = array(
          '#type' => 'select',
          '#title' => t('Facet field'),
          '#options' => $this->getFieldOptions(),
          '#default_value' => $this->get_option('facet_field'),
        );
        break;
      case 'use_more':
        $form['use_more']['#description'] = t('This will add a more link to the bottom of this view, which will link to the base path for the facet links.');
        $form['use_more_always'] = array(
          '#type' => 'value',
          '#value' => $this->get_option('use_more_always'),
        );
        break;
    }
  }

  public function options_validate(&$form, &$form_state) {
    if (substr($this->view->base_table, 0, 17) != 'search_api_index_') {
      form_set_error('', t('The "Facets block" display can only be used with base tables based on Search API indexes.'));
    }
  }

  public function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);

    switch ($form_state['section']) {
      case 'linked_path':
        $this->set_option('linked_path', $form_state['values']['linked_path']);
        break;
      case 'facet_field':
        $this->set_option('facet_field', $form_state['values']['facet_field']);
        break;
    }
  }

  public function options_summary(&$categories, &$options) {
    parent::options_summary($categories, $options);

    $options['linked_path'] = array(
      'category' => 'block',
      'title' => t('Search page path'),
      'value' => $this->get_option('linked_path') ? $this->get_option('linked_path') : t('Use current path'),
    );
    $field_options = $this->getFieldOptions();
    $options['facet_field'] = array(
      'category' => 'block',
      'title' => t('Facet field'),
      'value' => $this->get_option('facet_field') ? $field_options[$this->get_option('facet_field')] : t('None'),
    );
  }

  protected $field_options = NULL;

  protected function getFieldOptions() {
    if (!isset($this->field_options)) {
      $index = search_api_index_load(substr($this->view->base_table, 17));
      $this->field_options = array();
      if (!empty($index->options['fields'])) {
        foreach ($index->options['fields'] as $key => $field) {
          if ($field['indexed']) {
            $this->field_options[$key] = $field['name'];
          }
        }
      }
    }
    return $this->field_options;
  }

  /**
   * Render the 'more' link
   */
  public function render_more_link() {
    if ($this->use_more()) {
      $path = $this->get_option('linked_path');
      $theme = views_theme_functions('views_more', $this->view, $this->display);
      $path = check_url(url($path, array()));

      return array(
        '#theme' => $theme,
        '#more_url' => $path,
        '#link_text' => check_plain($this->use_more_text()),
      );
    }
  }

  public function execute() {
    if (substr($this->view->base_table, 0, 17) != 'search_api_index_') {
      form_set_error('', t('The "Facets block" display can only be used with base tables based on Search API indexes.'));
      return NULL;
    }
    $facet_field = $this->get_option('facet_field');
    if (!$facet_field) {
      return NULL;
    }

    $base_path = $this->get_option('linked_path');
    if (!$base_path) {
      $base_path = $_GET['q'];
    }
    $this->view->build();
    $limit = empty($this->view->query->pager->options['items_per_page']) ? 10 : $this->view->query->pager->options['items_per_page'];
    $query_options = &$this->view->query->getOptions();
    $query_options['search_api_facets']['search_api_views_facets_block'] = array(
      'field' => $facet_field,
      'limit' => $limit,
      'missing' => FALSE,
      'min_count' => 1,
    );
    $query_options['search id'] = 'search_api_views:' . $this->view->name . '-facets_block';
    $this->view->query->range(0, 0);

    $this->view->execute();
    $results = $this->view->query->getSearchApiResults();

    if (empty($results['search_api_facets']['search_api_views_facets_block'])) {
      return NULL;
    }

    $facet = new SearchApiFacet(array(
      'index_id' => substr($this->view->base_table, 17),
      'field' => $facet_field,
      'options' => array(),
    ));
    _search_api_facets_refresh_type($facet, $facet->options);
    $facet->options += array('options' => array());

    $theme_suffix  = '';
    $theme_suffix .= '__' . preg_replace('/\W+/', '_', $this->view->query->getIndex()->entity_type);
    $theme_suffix .= '__' . preg_replace('/\W+/', '_', $facet_field);
    $theme_suffix .= '__search_api_views_facets_block';
    $info['content']['facets'] = array(
      '#theme'  => 'search_api_facets_list' . $theme_suffix,
      '#terms'  => array(),
    );
    foreach ($results['search_api_facets']['search_api_views_facets_block'] as $term) {
      $query = array();
      $query['filter'][$facet_field][0] = $term['filter'];
      $info['content']['facets']['#terms'][] = array(
        '#theme'   => 'search_api_facets_facet' . $theme_suffix,
        '#name'    => _search_api_create_filter_name($term['filter'], $facet->options['type'], $facet->options['options']),
        '#count'   => $term['count'],
        '#path'    => $base_path,
        '#options' => array('query' => $query),
        '#active'  => FALSE,
      );
    }
    $info['content']['more'] = $this->render_more_link();
    $info['subject'] = filter_xss_admin($this->view->get_title());
    return $info;
  }

}
