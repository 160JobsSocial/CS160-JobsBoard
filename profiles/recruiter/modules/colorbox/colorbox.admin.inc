<?php

/**
 * @file
 * Administrative page callbacks for the colorbox module.
 */

/**
 * General configuration form for controlling the colorbox behaviour.
 */
function colorbox_admin_settings() {
  drupal_add_js(drupal_get_path('module', 'colorbox') . '/js/colorbox_admin_settings.js', array('preprocess' => FALSE));

  $colorbox_path = variable_get('colorbox_path', COLORBOX_PATH);

  if (module_exists('image')) {
    $form['colorbox_imagefield'] = array(
      '#type' => 'fieldset',
      '#title' => t('Image field options (CCK)')
    );
    $form['colorbox_imagefield']['colorbox_imagefield_gallery'] = array(
      '#type' => 'radios',
      '#title' => t('Image field gallery'),
      '#default_value' => variable_get('colorbox_imagefield_gallery', 1),
      '#options' => array(0 => t('Per page gallery'), 1 => t('Per post gallery'), 2 => t('Per field gallery'), 3 => t('No gallery')),
      '#description' => t('Should the gallery be images within a single field, a single post (default) or all images on the page. The last option disables galleries.'),
    );
  }

  if (module_exists('image_old')) {
    $form['colorbox_image_module'] = array(
      '#type' => 'fieldset',
      '#title' => t('Image module settings'),
    );
    $form['colorbox_image_module']['colorbox_auto_image_nodes'] = array(
      '#type' => 'radios',
      '#title' => t('Image module integration'),
      '#options' => array(0 => t('Disabled'), 1 => t('Enabled')),
      '#default_value' => variable_get('colorbox_auto_image_nodes', 0),
      '#description' => t('Select if Colorbox should be activated for the Image module.'),
    );
    $derivative_options = array();
    $sizes = image_get_sizes();
    foreach ($sizes as $label => $size) {
      $derivative_options[$label] = $size['label'];
    }
    $form['colorbox_image_module']['colorbox_image_derivative'] = array(
      '#type' => 'select',
      '#title' => t('Image derivative'),
      '#options' => $derivative_options,
      '#default_value' => variable_get('colorbox_image_derivative', 'preview'),
      '#description' => t('Select which image derivative to load in the Colorbox.'),
    );
  }

  if (module_exists('insert')) {
    $form['colorbox_insert_module'] = array(
      '#type' => 'fieldset',
      '#title' => t('Insert module settings'),
    );
    $image_styles = image_style_options(FALSE);
    $form['colorbox_insert_module']['colorbox_image_style'] = array(
      '#type' => 'select',
      '#title' => t('Image style'),
      '#empty_option' => t('None (original image)'),
      '#options' => $image_styles,
      '#default_value' => variable_get('colorbox_image_style', ''),
      '#description' => t('Select which image style to use for viewing images in the colorbox.'),
    );
    $form['colorbox_insert_module']['colorbox_insert_gallery'] = array(
      '#type' => 'radios',
      '#title' => t('Insert image gallery'),
      '#default_value' => variable_get('colorbox_insert_gallery', 0),
      '#options' => array(0 => t('Per page gallery'), 3 => t('No gallery')),
      '#description' => t('Should the gallery be all images on the page (default) or disabled.'),
    );
  }

  $form['colorbox_login_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Login links settings')
  );
  $form['colorbox_login_settings']['colorbox_login'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for login links'),
    '#default_value' => variable_get('colorbox_login', 0),
    '#description' => t('Automatically activate Colorbox for links to user/login.'),
  );
  $form['colorbox_login_settings']['colorbox_login_form'] = array(
    '#type' => 'select',
    '#title' => t('Login form to use'),
    '#options' => array('user_login' => t('User login'), 'user_login_block' => t('User login block')),
    '#default_value' => variable_get('colorbox_login_form', 'user_login'),
    '#description' => t('Select which form to use for login.'),
    '#states' => array(
      'visible' => array(
        ':input[name="colorbox_login"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['colorbox_extra_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Extra settings'),
  );
  $form['colorbox_extra_settings']['colorbox_load'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Colorbox load'),
    '#default_value' => variable_get('colorbox_load', 0),
    '#description' => t('This enables custom links that can open forms and paths in a Colorbox. Add the class "colorbox-load" to the link and build the url like this for forms "/colorbox/form/[form_id]?width=500&height=500" and like this for paths "[path]?width=500&height=500&iframe=true". If you do not want the box to persist set iframe to false. Other modules may activate this for easy Colorbox integration.'),
  );
  $form['colorbox_extra_settings']['colorbox_inline'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Colorbox inline'),
    '#default_value' => variable_get('colorbox_inline', 0),
    '#description' => t('This enables custom links that can open inline content in a Colorbox. Add the class "colorbox-inline" to the link and build the url like this "?width=500&height=500&inline=true#id-of-content". Other modules may activate this for easy Colorbox integration.'),
  );

  $form['colorbox_custom_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Styles and options'),
  );
  $colorbox_styles = array(
    'default' => t('Default'),
    'stockholmsyndrome' => t('Stockholm Syndrome'),
    $colorbox_path . '/example1' => t('Example 1'),
    $colorbox_path . '/example2' => t('Example 2'),
    $colorbox_path . '/example3' => t('Example 3'),
    $colorbox_path . '/example4' => t('Example 4'),
    $colorbox_path . '/example5' => t('Example 5'),
    'none' => t('None'),
  );
  $form['colorbox_custom_settings']['colorbox_style'] = array(
    '#type' => 'select',
    '#title' => t('Style'),
    '#options' => $colorbox_styles,
    '#default_value' => variable_get('colorbox_style', 'default'),
    '#description' => t('Select the style to use for the Colorbox. The example styles are the ones that come with the Colorbox plugin. Select "None" if you have added Colorbox styles to your theme.'),
  );
  $form['colorbox_custom_settings']['colorbox_custom_settings_activate'] = array(
    '#type' => 'radios',
    '#title' => t('Options'),
    '#options' => array(0 => t('Default'), 1 => t('Custom')),
    '#default_value' => variable_get('colorbox_custom_settings_activate', 0),
    '#description' => t('Use the default or custom options for Colorbox.'),
    '#prefix' => '<div class="colorbox-custom-settings-activate">',
    '#suffix' => '</div>',
  );

  $js_hide = variable_get('colorbox_custom_settings_activate', 0) ? '' : ' js-hide';
  $form['colorbox_custom_settings']['wrapper_start'] = array(
    '#markup' => '<div class="colorbox-custom-settings' . $js_hide . '">',
  );

  $form['colorbox_custom_settings']['colorbox_transition_type'] = array(
    '#type' => 'radios',
    '#title' => t('Transition type'),
    '#options' => array('elastic' => t('Elastic'), 'fade' => t('Fade'), 'none' => t('None')),
    '#default_value' => variable_get('colorbox_transition_type', 'elastic'),
    '#description' => t('The transition type.'),
  );
  $form['colorbox_custom_settings']['colorbox_transition_speed'] = array(
    '#type' => 'select',
    '#title' => t('Transition speed'),
    '#options' => drupal_map_assoc(array(100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600)),
    '#default_value' => variable_get('colorbox_transition_speed', 350),
    '#description' => t('Sets the speed of the fade and elastic transitions, in milliseconds.'),
  );
  $form['colorbox_custom_settings']['colorbox_opacity'] = array(
    '#type' => 'select',
    '#title' => t('Opacity'),
    '#options' => drupal_map_assoc(array('0', '0.10', '0.15', '0.20', '0.25', '0.30', '0.35', '0.40', '0.45', '0.50', '0.55', '0.60', '0.65', '0.70', '0.75', '0.80', '0.85', '0.90', '0.95', '1')),
    '#default_value' => variable_get('colorbox_opacity', '0.90'),
    '#description' => t('The overlay opacity level. Range: 0 to 1.'),
  );
  $form['colorbox_custom_settings']['colorbox_text_current'] = array(
    '#type' => 'textfield',
    '#title' => t('Current'),
    '#default_value' => variable_get('colorbox_text_current', '{current} of {total}'),
    '#size' => 30,
    '#description' => t('Text format for the content group / gallery count. {current} and {total} are detected and replaced with actual numbers while Colorbox runs.'),
  );
  $form['colorbox_custom_settings']['colorbox_text_previous'] = array(
    '#type' => 'textfield',
    '#title' => t('Previous'),
    '#default_value' => variable_get('colorbox_text_previous', '« Prev'),
    '#size' => 30,
    '#description' => t('Text for the previous button in a shared relation group.'),
  );
  $form['colorbox_custom_settings']['colorbox_text_next'] = array(
    '#type' => 'textfield',
    '#title' => t('Next'),
    '#default_value' => variable_get('colorbox_text_next', 'Next »'),
    '#size' => 30,
    '#description' => t('Text for the next button in a shared relation group.'),
  );
  $form['colorbox_custom_settings']['colorbox_text_close'] = array(
    '#type' => 'textfield',
    '#title' => t('Close'),
    '#default_value' => variable_get('colorbox_text_close', 'Close'),
    '#size' => 30,
    '#description' => t('Text for the close button. The "Esc" key will also close Colorbox.'),
  );
  $form['colorbox_custom_settings']['colorbox_overlayclose'] = array(
    '#type' => 'checkbox',
    '#title' => t('Overlay close'),
    '#default_value' => variable_get('colorbox_overlayclose', 1),
    '#description' => t('Enable closing ColorBox by clicking on the background overlay.'),
  );
  $form['colorbox_custom_settings']['colorbox_maxwidth'] = array(
    '#type' => 'textfield',
    '#title' => t('Max width'),
    '#default_value' => variable_get('colorbox_maxwidth', '100%'),
    '#size' => 30,
    '#description' => t('Set a maximum width for loaded content. Example: "100%", 500, "500px".'),
  );
  $form['colorbox_custom_settings']['colorbox_maxheight'] = array(
    '#type' => 'textfield',
    '#title' => t('Max height'),
    '#default_value' => variable_get('colorbox_maxheight', '100%'),
    '#size' => 30,
    '#description' => t('Set a maximum height for loaded content. Example: "100%", 500, "500px".'),
  );

  $form['colorbox_custom_settings']['colorbox_slideshow_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Slideshow settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_slideshow'] = array(
    '#type' => 'radios',
    '#title' => t('Slideshow'),
    '#options' => array(0 => t('Off'), 1 => t('On')),
    '#default_value' => variable_get('colorbox_slideshow', 0),
    '#description' => t('An automatic slideshow to a content group / gallery.'),
    '#prefix' => '<div class="colorbox-slideshow-settings-activate">',
    '#suffix' => '</div>',
  );

  $js_hide = variable_get('colorbox_slideshow', 0) ? '' : ' js-hide';
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['wrapper_start'] = array(
    '#markup' => '<div class="colorbox-slideshow-settings' . $js_hide . '">',
  );

  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_slideshowauto'] = array(
    '#type' => 'checkbox',
    '#title' => t('Slideshow autostart'),
    '#default_value' => variable_get('colorbox_slideshowauto', 1),
    '#description' => t('If the slideshow should automatically start to play.'),
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_slideshowspeed'] = array(
    '#type' => 'select',
    '#title' => t('Slideshow speed'),
    '#options' => drupal_map_assoc(array(1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000)),
    '#default_value' => variable_get('colorbox_slideshowspeed', 2500),
    '#description' => t('Sets the speed of the slideshow, in milliseconds.'),
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_text_start'] = array(
    '#type' => 'textfield',
    '#title' => t('Start slideshow'),
    '#default_value' => variable_get('colorbox_text_start', 'start slideshow'),
    '#size' => 30,
    '#description' => t('Text for the slideshow start button.'),
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_text_stop'] = array(
    '#type' => 'textfield',
    '#title' => t('Stop slideshow'),
    '#default_value' => variable_get('colorbox_text_stop', 'stop slideshow'),
    '#size' => 30,
    '#description' => t('Text for the slideshow stop button.'),
  );

  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['wrapper_stop'] = array(
    '#markup' => '</div>',
  );

  $form['colorbox_custom_settings']['wrapper_stop'] = array(
    '#markup' => '</div>',
  );

  $form['colorbox_advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  $form['colorbox_advanced_settings']['colorbox_title_trim'] = array(
    '#type' => 'radios',
    '#title' => t('Title shortening'),
    '#options' => array(0 => t('Default'), 1 => t('Yes')),
    '#default_value' => variable_get('colorbox_title_trim', 0),
    '#description' => t('If the title should be made shorter in the Colorbox to avoid layout problems. The default is to shorten for the example styles, they need it, but not for other styles.'),
    '#prefix' => '<div class="colorbox-title-trim-settings-activate">',
    '#suffix' => '</div>',
  );
  $js_hide = variable_get('colorbox_title_trim', 0) ? '' : ' js-hide';
  $form['colorbox_advanced_settings']['colorbox_title_trim_length'] = array(
    '#type' => 'select',
    '#title' => t('Title max length'),
    '#options' => drupal_map_assoc(array(40, 45, 50, 55, 60, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120)),
    '#default_value' => variable_get('colorbox_title_trim_length', 75),
    '#prefix' => '<div class="colorbox-title-trim-settings' . $js_hide . '">',
    '#suffix' => '</div>',
  );
  $form['colorbox_advanced_settings']['colorbox_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to Colorbox'),
    '#default_value' => $colorbox_path,
    '#description' => t('Enter the path relative to Drupal root where the Colorbox plugin directory is located. NO trailing slash!'),
  );
  $form['colorbox_advanced_settings']['colorbox_compression_type'] = array(
    '#type' => 'radios',
    '#title' => t('Choose Colorbox compression level'),
    '#options' => array(
      'min' => t('Production (Minified)'),
      'none' => t('Development (Uncompressed Code)'),
    ),
    '#default_value' => variable_get('colorbox_compression_type', 'min'),
  );
  $form['colorbox_advanced_settings']['colorbox_pages'] = array(
    '#type' => 'textarea',
    '#title' => t('Deactivate Colorbox on specific pages'),
    '#default_value' => variable_get('colorbox_pages', "admin*\nimg_assist*\nimce*\nnode/add/*\nnode/*/edit"),
    '#description' => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>')),
  );

  return system_settings_form($form);
}

