<?php

/**
 * Implementation of hook_mail().
 */
function lab_migration_mail($key, &$message, $params)
{
  global $user;
  $language = $message['language'];
  switch ($key)
  {
    case 'proposal_received':
      /* initializing data */
      $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $params['proposal_received']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);

      $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number", $params['proposal_received']['proposal_id'], 1);
      $experiment_list = '
      	';
      while ($experiment_data = db_fetch_object($experiment_q)) {
      	$experiment_list .= $experiment_data->number . ' ' . $experiment_data->title . '
      	';
      }

      $user_data = user_load($params['proposal_received']['user_id']);
 
      $message['subject'] = t('[!site_name] Your Lab migration proposal has been received', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

We have received your following Lab migration proposal:

Full Name : ' . $proposal_data->name_title . ' ' . $proposal_data->name . '
Email : ' . $user_data->mail . '
Contact Phone : ' . $proposal_data->contact_ph . '
Department/Branch : ' . $proposal_data->department . '
University/Institute : ' . $proposal_data->university . '

List of experiments : ' . $experiment_list . '

Your proposal is under review and you will soon receive an email from us regarding the same.

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'proposal_disapproved':
      /* initializing data */
      $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $params['proposal_disapproved']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);
      $preference1_q = db_query("SELECT * FROM {lab_migration_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_disapproved']['proposal_id'], 1);
      $preference1_data = db_fetch_object($preference1_q);
      $preference2_q = db_query("SELECT * FROM {lab_migration_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_disapproved']['proposal_id'], 2);
      $preference2_data = db_fetch_object($preference2_q);
      $preference3_q = db_query("SELECT * FROM {lab_migration_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $params['proposal_disapproved']['proposal_id'], 3);
      $preference3_data = db_fetch_object($preference3_q);
      $user_data = user_load($params['proposal_disapproved']['user_id']);

      $message['subject'] = t('[!site_name] Your book proposal has been disapproved', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Your following book proposal has been disapproved:

Reason for disapproval: ' . $proposal_data->message . '

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' . $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' .  $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' .  $proposal_data->reviewer . '
Expected date of completion : ' .  date('d-m-Y', $proposal_data->completion_date) . '

Your Book Preferences :

Book Preference 1 :-
Title of the book : ' . $preference1_data->book . '
Author name : ' . $preference1_data->author . '
ISBN No. : ' . $preference1_data->isbn . '
Publisher and Place : ' . $preference1_data->publisher . '
Edition : ' . $preference1_data->edition . '
Year of publication : ' . $preference1_data->year . '

Book Preference 2 :-
Title of the book : ' . $preference2_data->book . '
Author name : ' . $preference2_data->author . '
ISBN No. : ' . $preference2_data->isbn . '
Publisher and Place : ' . $preference2_data->publisher . '
Edition : ' . $preference2_data->edition . '
Year of publication : ' . $preference2_data->year . '

Book Preference 3 :-
Title of the book : ' . $preference3_data->book . '
Author name : ' . $preference3_data->author . '
ISBN No. : ' . $preference3_data->isbn . '
Publisher and Place : ' . $preference3_data->publisher . '
Edition : ' . $preference3_data->edition . '
Year of publication : ' . $preference3_data->year . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'proposal_approved':
      $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $params['proposal_approved']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);
      $approved_preference_q = db_query("SELECT * FROM {lab_migration_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $params['proposal_approved']['proposal_id']);
      $approved_preference_data = db_fetch_object($approved_preference_q);
      $user_data = user_load($params['proposal_approved']['user_id']);

      $message['subject'] = t('[!site_name] Your book proposal has been approved', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Your following book proposal has been approved:

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' .  $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' .  $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' .  $proposal_data->reviewer . '
Expected date of completion : ' .  date('d-m-Y', $proposal_data->completion_date) . '

Title of the book : ' . $approved_preference_data->book . '
Author name : ' . $approved_preference_data->author . '
ISBN No. : ' . $approved_preference_data->isbn . '
Publisher and Place : ' . $approved_preference_data->publisher . '
Edition : ' . $approved_preference_data->edition . '
Year of publication : ' . $approved_preference_data->year . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'proposal_completed':
      $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $params['proposal_completed']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);
      $approved_preference_q = db_query("SELECT * FROM {lab_migration_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $params['proposal_completed']['proposal_id']);
      $approved_preference_data = db_fetch_object($approved_preference_q);
      $user_data = user_load($params['proposal_completed']['user_id']);

      $message['subject'] = t('[!site_name] Congratulations for completion of the book.', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Following book has been completed sucessfully by you:

Full Name : ' . $proposal_data->full_name . '
Email : ' . $user_data->mail . '
Mobile : ' .  $proposal_data->mobile . '
Course : ' . $proposal_data->course . '
Department/Branch : ' . $proposal_data->branch . '
University/Institute : ' .  $proposal_data->university . '
College Teacher / Professor : ' . $proposal_data->faculty . '
Reviewer : ' .  $proposal_data->reviewer . '
Expected date of completion : ' .  date('d-m-Y', $proposal_data->completion_date) . '

Title of the book : ' . $approved_preference_data->book . '
Author name : ' . $approved_preference_data->author . '
ISBN No. : ' . $approved_preference_data->isbn . '
Publisher and Place : ' . $approved_preference_data->publisher . '
Edition : ' . $approved_preference_data->edition . '
Year of publication : ' . $approved_preference_data->year . '

Your book is now available at following link to download.

http://scilab.in/textbook_run/' . $approved_preference_data->id . '

Now you should be able to propose a new book...

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'example_uploaded':
      $example_q = db_query("SELECT * FROM {lab_migration_example} WHERE id = %d LIMIT 1", $params['example_uploaded']['example_id']);
      $example_data = db_fetch_object($example_q);
      $user_data = user_load($params['example_uploaded']['user_id']);

      $message['subject'] = t('[!site_name] You have uploaded example', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

You have uploaded the following example:

Example number : ' . $example_data->number . '
Caption : ' .  $example_data->caption . '

The example is under review. You will be notified when it has been approved.

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'example_updated':
      $example_q = db_query("SELECT * FROM {lab_migration_example} WHERE id = %d LIMIT 1", $params['example_updated']['example_id']);
      $example_data = db_fetch_object($example_q);
      $user_data = user_load($params['example_updated']['user_id']);

      $message['subject'] = t('[!site_name] You have updated example', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

You have updated the following example:

Example number : ' . $example_data->number . '
Caption : ' .  $example_data->caption . '

The example is still under review. You will be notified when it has been approved.

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'example_updated_admin':
      $example_q = db_query("SELECT * FROM {lab_migration_example} WHERE id = %d LIMIT 1", $params['example_updated_admin']['example_id']);
      $example_data = db_fetch_object($example_q);
      $user_data = user_load($params['example_updated_admin']['user_id']);

      $message['subject'] = t('[!site_name] Reviewer have updated example', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Reviewer have updated the following example:

Example number : ' . $example_data->number . '
Caption : ' .  $example_data->caption . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'example_approved':
      $example_q = db_query("SELECT * FROM {lab_migration_example} WHERE id = %d LIMIT 1", $params['example_approved']['example_id']);
      $example_data = db_fetch_object($example_q);
      $user_data = user_load($params['example_approved']['user_id']);

      $message['subject'] = t('[!site_name] Your uploaded example has been approved', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Your following example has been approved:

Example number : ' . $example_data->number . '
Caption : ' .  $example_data->caption . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'example_disapproved':
      $user_data = user_load($params['example_disapproved']['user_id']);

      $message['subject'] = t('[!site_name] Your uploaded example has been disapproved', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Your following example has been disapproved:

Example number : ' . $params['example_disapproved']['example_number'] . '
Caption : ' . $params['example_disapproved']['example_caption'] . '

Reason for dis-approval : ' . $params['example_disapproved']['message'] . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'example_deleted_user':
      $user_data = user_load($params['example_deleted_user']['user_id']);

      $message['subject'] = t('[!site_name] User has deleted pending example', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Your following pending example has been deleted :

Title of the Book : ' . $params['example_deleted_user']['book_title'] . '
Title of the Chapter : ' . $params['example_deleted_user']['chapter_title'] . '
Example number : ' . $params['example_deleted_user']['example_number'] . '
Caption : ' . $params['example_deleted_user']['example_caption'] . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'dependency_uploaded':
      $user_data = user_load($params['dependency_uploaded']['user_id']);
      $dependency_files = implode(',', $params['dependency_uploaded']['dependency_names']);

      $message['subject'] = t('[!site_name] You have uploaded dependency file', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

You have uploaded following dependency files :
    ' . $dependency_files . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'feedback_received':
      $user_data = user_load($params['feedback_received']['user_id']);

      $message['subject'] = t('[!site_name] We have received your feedback', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

We have received your following feedback

Title of the Book: ' . $params['feedback_received']['book_title'] . '
Title of the Chapter: ' . $params['feedback_received']['chapter_number'] . ' ' . $params['feedback_received']['chapter_title'] . '
Example No.: ' . $params['feedback_received']['example_no'] . '

Your feedback :
' . $params['feedback_received']['feedback'] . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'standard':
      $message['subject'] = $params['standard']['subject'];
      $message['body'] = $params['standard']['body'];
      break;
  }
}

