<?php
/**
 * Implements hook_drush_command().
 */
function calendar_events_drush_command(){
     $items['cal-fetch'] = array(
      'description' => 'Fetches Calendar Events.',
      'aliases' => array('calf'),
      'drush cal-fetch' => 'Triggers the internal note and tags for ZenDesk'
    );
     $items['cal-send'] = array(
      'description' => 'Sends Calendar Events.',
      'aliases' => array('cals'),
      'drush cal-send' => 'Triggers the internal note and tags for ZenDesk'
    );
   return $items;
}
