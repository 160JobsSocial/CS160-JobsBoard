<?php

/**
 * @file
 * Link base test file - contains common functions for testing links.
 */

class LinkBaseTestClass extends DrupalWebTestCase {
  public $permissions = array(
    'access content',
    'administer content types',
    'administer nodes',
    'administer filters',
    'access comments',
    'post comments',
    'access administration pages',
    'create page content',
  );
  
  public $account;
  
  function setUp($modules = array()) {
    if ($modules) {
      parent::setUp($modules);
    }
    else {
      parent::setUp('field_ui', 'link');
    }
    $this->account = $this->drupalCreateUser($this->permissions);
    $this->drupalLogin($this->account);
  }
  
  function createLinkField($node_type = 'page',
                           $settings = array()) {
    $name = strtolower($this->randomName());
    $edit = array(
      'fields[_add_new_field][label]' => $name,
      'fields[_add_new_field][field_name]' => $name,
      'fields[_add_new_field][type]' => 'link_field',
      'fields[_add_new_field][widget_type]' => 'link_field',
    );
    $field_name = 'field_'. $name;
    $this->drupalPost('admin/structure/types/manage/'. $node_type .'/fields', $edit, t('Save'));
    $this->drupalPost(NULL, array(), t('Save field settings'));
    $this->drupalPost(NULL, $settings, t('Save settings'));

    // Is field created?
    $this->assertRaw(t('Saved %label configuration', array('%label' => $name)), 'Field added');
    node_types_rebuild();
    menu_rebuild();
    
    return $field_name;
  }
}