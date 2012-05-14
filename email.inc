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

      if ($proposal_data->solution_display == 1) {
      	$solution_display = 'Yes';
      } else {
      	$solution_display = 'No';
      }

			if ($proposal_data->solution_provider_uid == 0) {
				$solution_provider_user = 'Open';
			} else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
				$solution_provider_user = 'Proposer';
			} else {
				$solution_provider_user = 'Unknown';
			}

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

Display solution on website : ' . $solution_display . '

Solution Provided By : ' . $solution_provider_user . '

List of experiments : ' . $experiment_list . '

Your proposal is under review and you will soon receive an email from us regarding the same.

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'proposal_disapproved':

      /* initializing data */
      $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $params['proposal_disapproved']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);

      if ($proposal_data->solution_display == 1) {
      	$solution_display = 'Yes';
      } else {
      	$solution_display = 'No';
      }

			if ($proposal_data->solution_provider_uid == 0) {
				$solution_provider_user = 'Open';
			} else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
				$solution_provider_user = 'Proposer';
			} else {
				$solution_provider_user = 'Unknown';
			}

      $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number", $params['proposal_disapproved']['proposal_id'], 1);
      $experiment_list = '
      	';
      while ($experiment_data = db_fetch_object($experiment_q)) {
      	$experiment_list .= $experiment_data->number . ' ' . $experiment_data->title . '
      	';
      }

      $user_data = user_load($params['proposal_disapproved']['user_id']);

      $message['subject'] = t('[!site_name] Your Lab migration proposal has been disapproved', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Your following Lab migration proposal has been disapproved:

Reason for disapproval: ' . $proposal_data->message . '

Full Name : ' . $proposal_data->name_title . ' ' . $proposal_data->name . '
Email : ' . $user_data->mail . '
Contact Phone : ' . $proposal_data->contact_ph . '
Department/Branch : ' . $proposal_data->department . '
University/Institute : ' . $proposal_data->university . '

Display solution on website : ' . $solution_display . '

Solution Provided By : ' . $solution_provider_user . '

List of experiments : ' . $experiment_list . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'proposal_approved':

      /* initializing data */
      $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $params['proposal_disapproved']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);

      if ($proposal_data->solution_display == 1) {
      	$solution_display = 'Yes';
      } else {
      	$solution_display = 'No';
      }

			if ($proposal_data->solution_provider_uid == 0) {
				$solution_provider_user = 'Open';
			} else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
				$solution_provider_user = 'Proposer';
			} else {
				$solution_provider_user = 'Unknown';
			}

      $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number", $params['proposal_disapproved']['proposal_id'], 1);
      $experiment_list = '
      	';
      while ($experiment_data = db_fetch_object($experiment_q)) {
      	$experiment_list .= $experiment_data->number . ' ' . $experiment_data->title . '
      	';
      }

      $user_data = user_load($params['proposal_disapproved']['user_id']);

      $message['subject'] = t('[!site_name] Your Lab migration proposal has been approved', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Your following Lab migration proposal has been approved:

Full Name : ' . $proposal_data->name_title . ' ' . $proposal_data->name . '
Email : ' . $user_data->mail . '
Contact Phone : ' . $proposal_data->contact_ph . '
Department/Branch : ' . $proposal_data->department . '
University/Institute : ' . $proposal_data->university . '

Display solution on website : ' . $solution_display . '

Solution Provided By : ' . $solution_provider_user . '

List of experiments : ' . $experiment_list . '

Best Wishes,

!site_name', array('!site_name' => variable_get('site_name', ''), '!user_name' => $user_data->name), $language->language);
      break;

    case 'proposal_completed':

      /* initializing data */
      $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $params['proposal_completed']['proposal_id']);
      $proposal_data = db_fetch_object($proposal_q);

      if ($proposal_data->solution_display == 1) {
      	$solution_display = 'Yes';
      } else {
      	$solution_display = 'No';
      }

			if ($proposal_data->solution_provider_uid == 0) {
				$solution_provider_user = 'Open';
			} else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
				$solution_provider_user = 'Proposer';
			} else {
				$user_data = user_load($proposal_data->solution_provider_uid);
				if (!$user_data) {
					$solution_provider_user = 'Unknown';
				}
			}

      $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number", $params['proposal_completed']['proposal_id'], 1);
      $experiment_list = '
      	';
      while ($experiment_data = db_fetch_object($experiment_q)) {
      	$experiment_list .= $experiment_data->number . ' ' . $experiment_data->title . '
      	';
      }

      $user_data = user_load($params['proposal_completed']['user_id']);

      $message['subject'] = t('[!site_name] Congratulations for completion of the Lab migration', array('!site_name' => variable_get('site_name', '')), $language->language);
      $message['body'] = t('
Dear !user_name,

Following Lab migration has been completed sucessfully :

Full Name : ' . $proposal_data->name_title . ' ' . $proposal_data->name . '
Email : ' . $user_data->mail . '
Contact Phone : ' . $proposal_data->contact_ph . '
Department/Branch : ' . $proposal_data->department . '
University/Institute : ' . $proposal_data->university . '

Display solution on website : ' . $solution_display . '

Solution Provided By : ' . $solution_provider_user . '

List of experiments : ' . $experiment_list . '

Your Lab solution is now available at following link to download.

http://scilab.in/lab_migration_run/' . $proposal_data->id . '

Now you should be able to propose a new Lab migration...

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

    case 'standard':
      $message['subject'] = $params['standard']['subject'];
      $message['body'] = $params['standard']['body'];
      break;
  }
}

