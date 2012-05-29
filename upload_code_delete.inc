<?php

/******************************************************************************/
/***************************** DELETE CODE ************************************/
/******************************************************************************/

function lab_migration_upload_code_delete()
{
  global $user;

  $root_path = lab_migration_path();
  $solution_id = (int)arg(3);

  /* check solution */
  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE id = %d LIMIT 1", $solution_id);
  $solution_data = db_fetch_object($solution_q);
  if (!$solution_data)
  {
    drupal_set_message('Invalid solution.', 'error');
    drupal_goto('lab_migration/code');
    return;
  }
  if ($solution_data->approval_status != 0)
  {
    drupal_set_message('You cannnot delete a solution after it has been approved. Please contact site administrator if you want to delete this solution.', 'error');
    drupal_goto('lab_migration/code');
    return;
  }

  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d LIMIT 1", $solution_data->experiment_id);
  $experiment_data = db_fetch_object($experiment_q);
  if (!$experiment_data)
  {
    drupal_set_message('You do not have permission to delete this solution.', 'error');
    drupal_goto('lab_migration/code');
    return;
  }

  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d AND solution_provider_uid = %d LIMIT 1", $experiment_data->proposal_id, $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message('You do not have permission to delete this solution.', 'error');
    drupal_goto('lab_migration/code');
    return;
  }

  /* deleting solution files */
  if (delete_solution($solution_data->id))
  {
    drupal_set_message('Solution deleted.', 'status');

    /* sending email */
    $email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');
    $param['solution_deleted_user']['lab_title'] = $proposal_data->lab_title;
    $param['solution_deleted_user']['experiment_title'] = $experiment_data->title;
    $param['solution_deleted_user']['solution_number'] = $solution_data->code_number;
    $param['solution_deleted_user']['solution_caption'] = $solution_data->caption;
    $param['example_deleted_user']['user_id'] = $user->uid;

    if (!drupal_mail('lab_migration', 'solution_deleted_user', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');
  } else {
    drupal_set_message('Error deleting example.', 'status');
  }

  drupal_goto('lab_migration/code');
  return;
}

?>
