<?php

/******************************************************************************/
/****************************** DELETION FUNCTIONS ****************************/
/******************************************************************************/

function delete_solution($solution_id)
{
  global $user;
  $root_path = lab_migration_path();
  $status = TRUE;

  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE id = %d", $solution_id);
  $solution_data = db_fetch_object($solution_q);
  if (!$solution_data)
  {
    drupal_set_message(t('Invalid solution.'), 'error');
    return FALSE;
  }

  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $solution_data->experiment_id);
  $experiment_data = db_fetch_object($experiment_q);
  if (!$experiment_data)
  {
    drupal_set_message(t('Invalid experiment.'), 'error');
    return FALSE;
  }

  /* deleting solution files */
  $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_id);
  while ($solution_files_data = db_fetch_object($solution_files_q))
  {
    if (!file_exists($root_path . $solution_files_data->filepath))
    {
      $status = FALSE;
      drupal_set_message(t('Error deleting !file. File does not exists.', array('!file' => $solution_files_data->filepath)), 'error');
      continue;
    }

    /* removing solution file */
    if (!unlink($root_path . $solution_files_data->filepath))
    {
      $status = FALSE;
      drupal_set_message(t('Error deleting !file', array('!file' => $solution_files_data->filepath)), 'error');

      /* sending email to admins */
      $email_to = variable_get('lab_migration_emails', '');
      $param['standard']['subject'] = "[ERROR] Error deleting example file";
      $param['standard']['body'] = "Error deleting solution files by " . $user->uid . " at " . $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'] . " :
        solution id : " . $solution_id . "
        file id : " .  $solution_files_data->id . "
        file path : " . $solution_files_data->filepath;
      if (!drupal_mail('lab_migration', 'standard', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
        drupal_set_message('Error sending email message.', 'error');
    } else {
      /* deleting example files database entries */
      db_query("DELETE FROM {lab_migration_solution_files} WHERE id = %d", $solution_files_data->id);
    }
  }

  if (!$status)
    return FALSE;

  /* removing code folder */
  $ex_path = $experiment_data->proposal_id . '/EXP' . $experiment_data->number . '/CODE' . $solution_data->code_number;
  $dir_path = $root_path . $ex_path;
  if (is_dir($dir_path))
  {
    if (!rmdir($dir_path))
    {
      drupal_set_message(t('Error deleting folder !folder', array('!folder' => $dir_path)), 'error');

      /* sending email to admins */
      $email_to = variable_get('lab_migration_emails', '');
      $param['standard']['subject'] = "[ERROR] Error deleting folder";
      $param['standard']['body'] = "Error deleting folder " . $dir_path . " by " . $user->uid . " at " . $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'];
      if (!drupal_mail('lab_migration', 'standard', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
        drupal_set_message('Error sending email message.', 'error');
      return FALSE;
    }
  } else {
    drupal_set_message(t('Cannot delete solution folder. !folder does not exists.', array('!folder' => $dir_path)), 'error');
    return FALSE;
  }

  /* deleting solution dependency and solution database entries */
  db_query("DELETE FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_id);
  db_query("DELETE FROM {lab_migration_solution} WHERE id = %d", $solution_id);

  return $status;
}

