<?php
// $Id$

function lab_migration_download_full_experiment()
{
  $experiment_id = arg(3);
  $root_path = lab_migration_path();
  $APPROVE_PATH = 'APPROVED/';
  $PENDING_PATH = 'PENDING/';

  /* get solution data */
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $experiment_id);
  $experiment_data = db_fetch_object($experiment_q);
  $EXP_PATH = 'EXP' . $experiment_data->number . '/';

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

  /* approved solutions */
  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 1", $experiment_id);
  while ($solution_row = db_fetch_object($solution_q))
  {
    $CODE_PATH = 'CODE' . $solution_row->code_number . '/';
    $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_row->id);
    $solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_row->id);
    while ($solution_files_row = db_fetch_object($solution_files_q))
    {
      $zip->addFile($root_path . $solution_files_row->filepath, $APPROVE_PATH . $EXP_PATH . $CODE_PATH . $solution_files_row->filename);
    }
    /* dependency files */
    while ($solution_dependency_files_row = db_fetch_object($solution_dependency_files_q))
    {
      $dependency_file_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id));
      if ($dependency_file_data)
        $zip->addFile($root_path . $dependency_file_data->filepath, $APPROVE_PATH . $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
    }
  }

  /* unapproved solutions */
  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 0", $experiment_id);
  while ($solution_row = db_fetch_object($solution_q))
  {
    $CODE_PATH = 'CODE' . $solution_row->code_number . '/';
    $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_row->id);
    $solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_row->id);
    while ($solution_files_row = db_fetch_object($solution_files_q))
    {
      $zip->addFile($root_path . $solution_files_row->filepath, $PENDING_PATH . $EXP_PATH . $CODE_PATH . $solution_files_row->filename);
    }
    /* dependency files */
    while ($solution_dependency_files_row = db_fetch_object($solution_dependency_files_q))
    {
      $dependency_file_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id));
      if ($dependency_file_data)
        $zip->addFile($root_path . $dependency_file_data->filepath, $PENDING_PATH . $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
    }
  }

  $zip_file_count = $zip->numFiles;
  $zip->close();

  if ($zip_file_count > 0)
  {
    /* download zip file */
    header('Content-Type: application/zip');
    header('Content-disposition: attachment; filename="EXP' . $experiment_data->number . '.zip"');
    header('Content-Length: ' . filesize($zip_filename));
    readfile($zip_filename);
    unlink($zip_filename);
  } else {
    drupal_set_message("There are no solutions in this experiment to download", 'error');
    drupal_goto('lab_migration/code_approval/bulk');
  }
}

function lab_migration_download_full_lab()
{
  $lab_id = arg(3);
  $root_path = lab_migration_path();
  $APPROVE_PATH = 'APPROVED/';
  $PENDING_PATH = 'PENDING/';

  /* get solution data */
  $lab_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $lab_id);
  $lab_data = db_fetch_object($lab_q);
  $LAB_PATH = $lab_data->lab . '/';

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

  /* approved solutions */
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $lab_id);
  while ($experiment_row = db_fetch_object($experiment_q))
  {
    $EXP_PATH = 'EXP' . $experiment_row->number . '/';
    $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 1", $experiment_row->id);
    while ($solution_row = db_fetch_object($solution_q))
    {
      $CODE_PATH = 'CODE' . $solution_row->code_number . '/';
      $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_row->id);
      $solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_row->id);
      while ($solution_files_row = db_fetch_object($solution_files_q))
      {
        $zip->addFile($root_path . $solution_files_row->filepath, $LAB_PATH . $APPROVE_PATH . $EXP_PATH . $CODE_PATH . $solution_files_row->filename);
      }
      /* dependency files */
      while ($solution_dependency_files_row = db_fetch_object($solution_dependency_files_q))
      {
        $dependency_file_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id));
        if ($dependency_file_data)
          $zip->addFile($root_path . $dependency_file_data->filepath, $LAB_PATH . $APPROVE_PATH . $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
      }
    }

    /* unapproved solutions */
    $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 0", $experiment_row->id);
    while ($solution_row = db_fetch_object($solution_q))
    {
      $CODE_PATH = 'CODE' . $solution_row->code_number . '/';
      $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_row->id);
      $solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_row->id);
      while ($solution_files_row = db_fetch_object($solution_files_q))
      {
        $zip->addFile($root_path . $solution_files_row->filepath, $LAB_PATH . $PENDING_PATH . $EXP_PATH . $CODE_PATH . $solution_files_row->filename);
      }
      /* dependency files */
      while ($solution_dependency_files_row = db_fetch_object($solution_dependency_files_q))
      {
        $dependency_file_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id));
        if ($dependency_file_data)
          $zip->addFile($root_path . $dependency_file_data->filepath, $LAB_PATH . $PENDING_PATH . $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
      }
    }
  }

  $zip_file_count = $zip->numFiles;
  $zip->close();

  if ($zip_file_count > 0)
  {
    /* download zip file */
    header('Content-Type: application/zip');
    header('Content-disposition: attachment; filename="' . $lab_data->lab_title . '.zip"');
    header('Content-Length: ' . filesize($zip_filename));
    readfile($zip_filename);
    unlink($zip_filename);
  } else {
    drupal_set_message("There are no solutions in this lab to download", 'error');
    drupal_goto('lab_migration/code_approval/bulk');
  }
}

