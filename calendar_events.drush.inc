<?php
/**
 * Implements hook_drush_command().
 */
function drush_calendar_fetch(){
     $items['cal-eventsfetch'] = array(
      'description' => 'Fetches Calendar Events.',
      'aliases' => array('calf'),
      'drush cal-eventsfetch' => 'Triggers the internal note and tags for ZenDesk'
    );
   return $items;
}

function drush_calendar_send(){
     $items['cal-eventsend'] = array(
      'description' => 'Sends Calendar Events.',
      'aliases' => array('cals'),
      'drush cal-eventsend' => 'Triggers the internal note and tags for ZenDesk'
    );
   return $items;
}
