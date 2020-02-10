<?php

/**
 * @file
 * Create Calendar Drush Command.
 */

/**
 * Implements hook_drush_command().
 */
function calendar_events_drush_command(){
    $items['calendar-events-fetch'] = array(
      'description' => 'Fetches Calendar Events.',
      'aliases' => array('cef'),
      'drush calendar-events-fetch' => 'Triggers the internal note and tags for Zendesk',
    );
     $items['cal-send'] = array(
      'description' => 'Sends Calendar Events.',
      'aliases' => array('cals'),
      'drush cal-send' => 'Triggers the internal note and tags for ZenDesk'
    );
   return $items;
}

require_once DRUPAL_ROOT . '/sites/all/modules/custom/calendar_events/vendor/autoload.php';

function calendar_events_get_client() {
  putenv('GOOGLE_APPLICATION_CREDENTIALS=' . '/mnt/gfs/eearossrussell.prod/nobackup/apikeys/calendar-events-a63526983c2d.json');
  $client = new Google_Client();
  $scopes = array('https://www.googleapis.com/auth/calendar.events.readonly');
  $client->useApplicationDefaultCredentials();
  $client->setScopes($scopes);
  $service = new Google_Service_Calendar($client);
  return $service;
}
function calendar_events_get_gcal_events() {
  $service = calendar_events_get_client();
  $calendarId = 'acquia.com_up74cprjekio3bs72tsi7lqg14@group.calendar.google.com';
  $optParams = array(
    'maxResults' => 10,
    'orderBy' => 'startTime',
    'singleEvents' => true,
    'timeMin' => date("c", mktime(date("H")-12, date("i"), date("s"), date("m"), date("d"), date("Y"))),
    'timeMax' => date("c", mktime(date("H")+12, date("i"), date("s"), date("m"), date("d"), date("Y"))),
  );
  try {
      $results = $service->events->listEvents($calendarId, $optParams);
    }
    catch (ApiResponseException $cal_get_e) {
      watchdog_exception("calendar_events", $cal_get_e);
    }
    $events = $results->getItems();
    if (empty($events)) {
      drush_print("There are no current events in the Customer Events calendar right now.");
      return;
    }
    drush_print("There are current events in the Events Calendar right now.");
    $location_array = array();
    $creator_emails = array();
    drush_print("Calendar Events Found:");
    foreach ($events as $event) {
      array_push($location_array, $event->getLocation());
      drush_print(dt("Event Name: @event_summary", [
        '@event_summary' => $event->getSummary(),
      ]));
      // Get event creator information.
      $creator = $event->getCreator();
      array_push($creator_emails, $creator->email);
    }
    $validated_array = array();
    $user_error_position = array();
    foreach ($location_array as $key => $location_id) {
      // Zendesk ticket # Validation:
      // Validate the submission is a number.
      if (preg_match('/^\d+$/', $location_id)) {
        array_push($validated_array, $location_id);
        continue;
      }
      // If number is not a zendesk ticket number, print incorrect submission.
      drush_print(dt("Tickets have been submitted in the incorrect format @location_id", [
        '@location_id' => $location_id,
      ]));
      array_push($user_error_position, $key);
    }
    // Find email address of users with errors.
    foreach ($user_error_position as $array_position) {
      $email = $creator_emails[$array_position];
      drush_print(dt("User who inputted error is @user_email", [
        '@user_email' => $email,
      ]));
    }
    $cache_lifetime = variable_get('calendar_events_gcal_cache_lifetime');
    $current_time = time();
    // Set cache with results.
    cache_set('calendar_events_gcal_data', $validated_array, 'cache', $current_time + $cache_lifetime);
  }

// get_data_from_gcal();

function drush_calendar_events_fetch() {
  calendar_events_get_gcal_events();
  drush_print("Calendar events fetch function has completed.");
}

 function send_event_to_cal() {
   //putenv('GOOGLE_APPLICATION_CREDENTIALS=./sites/all/modules/custom/calendar_events/vendor/calendar-events-948c5f78be89.json');
   putenv('GOOGLE_APPLICATION_CREDENTIALS=' . DRUPAL_ROOT . '/sites/all/modules/custom/calendar_events/vendor/calendar-events-a06518c4d1b5.json');
   $client = new Google_Client();
   $scopes = array('https://www.googleapis.com/auth/calendar');
   $client->useApplicationDefaultCredentials();
   $client->setScopes($scopes);

   $service = new Google_Service_Calendar($client);

   // Get event notification zendesk tickets from cache
   $tickets_from_cache = cache_get('cached_event_tickets');
   $event_tickets = array_values($tickets_from_cache->data);
   $calendarId = 'acquia.com_up74cprjekio3bs72tsi7lqg14@group.calendar.google.com';

   foreach($event_tickets as $tickets=>$ticket_data) {
     $eventLocation = $ticket_data[0];
     $eventSummary = $ticket_data[1];
     $eventDescription = $ticket_data[2];
     $eventStart = $ticket_data[3];
     $eventEnd = $ticket_data[4];

     $eventParams = new Google_Service_Calendar_Event( array(
         'summary' => $eventSummary,
         'location' => $eventLocation,
         'description' => $eventDescription,
         'start' => array(
           // 'dateTime' => '2019-03-05T15:00:00-08:00',
           'dateTime' => $eventStart,
           'timeZone' => 'America/Los_Angeles',
         ),
         'end' => array(
           // 'dateTime' => '2019-03-05T17:00:00-08:00',
           'dateTime' => $eventEnd,
           'timeZone' => 'America/Los_Angeles',
         ),
       ));
     $event = $service->events->Insert($calendarId, $eventParams);
     printf('Event created: %s\n', $event->htmlLink);
   }
 }

 // send_event_to_cal();

 function drush_calendar_events_cal_send()
 {
     //GET Zendesk Tickets with Event Type
     send_event_to_cal();
     drush_print("calendar events send function run");
  }
