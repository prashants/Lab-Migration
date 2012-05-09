<?php
// $Id$

function lab_migration_download_solution_file()
{
  $solution_file_id = arg(3);
  $root_path = lab_migration_path();

  $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE id = %d LIMIT 1", $solution_file_id);
  $solution_file_data = db_fetch_object($solution_files_q);
  header('Content-Type: ' . $solution_file_data->filemime);
  header('Content-disposition: attachment; filename="' . $solution_file_data->filename . '"');
  header('Content-Length: ' . filesize($root_path . $solution_file_data->filepath));
  readfile($root_path . $solution_file_data->filepath);
}

function lab_migration_download_dependency_file()
{
  $dependency_file_id = arg(3);
  $root_path = lab_migration_path();

  $dependency_file_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $dependency_file_id);
  $dependency_file_data = db_fetch_object($dependency_file_q);
  header('Content-Type: ' . $dependency_file_data->filemime);
  header('Content-disposition: attachment; filename="' . $dependency_file_data->filename . '"');
  header('Content-Length: ' . filesize($root_path . $dependency_file_data->filepath));
  readfile($root_path . $dependency_file_data->filepath);
}

function lab_migration_download_solution()
{
  $solution_id = arg(3);
  $root_path = lab_migration_path();

  /* get solution data */
  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE id = %d", $solution_id);
  $solution_data = db_fetch_object($solution_q);
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $solution_data->experiment_id);
  $experiment_data = db_fetch_object($experiment_q);
  $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_id);
  $solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_id);

  $CODE_PATH = 'CODE' . $solution_data->code_number . '/';

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

  while ($solution_files_row = db_fetch_object($solution_files_q))
  {
      $zip->addFile($root_path . $solution_files_row->filepath, $CODE_PATH . $solution_files_row->filename);
  }
  /* dependency files */
  while ($solution_dependency_files_row = db_fetch_object($solution_dependency_files_q))
  {
    $dependency_file_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id));
    if ($dependency_file_data)
      $zip->addFile($root_path . $dependency_file_data->filepath, $CODE_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
  }
  $zip_file_count = $zip->numFiles;
  $zip->close();

  if ($zip_file_count > 0)
  {
    /* download zip file */
    header('Content-Type: application/zip');
    header('Content-disposition: attachment; filename="CODE' . $solution_data->code_number . '.zip"');
    header('Content-Length: ' . filesize($zip_filename));
    readfile($zip_filename);
    unlink($zip_filename);
  } else {
    drupal_set_message("There are no files in this solutions to download", 'error');
    drupal_goto('lab_migration_run');
  }
}

function lab_migration_download_experiment()
{
  $experiment_id = arg(3);
  $root_path = lab_migration_path();

  /* get solution data */
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $experiment_id);
  $experiment_data = db_fetch_object($experiment_q);
  $EXP_PATH = 'EXP' . $experiment_data->number . '/';

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 1", $experiment_id);
  while ($solution_row = db_fetch_object($solution_q))
  {
    $CODE_PATH = 'CODE' . $solution_row->code_number . '/';
    $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_row->id);
    $solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_row->id);
    while ($solution_files_row = db_fetch_object($solution_files_q))
    {
      $zip->addFile($root_path . $solution_files_row->filepath, $EXP_PATH . $CODE_PATH . $solution_files_row->filename);
    }
    /* dependency files */
    while ($solution_dependency_files_row = db_fetch_object($solution_dependency_files_q))
    {
      $dependency_file_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id));
      if ($dependency_file_data)
        $zip->addFile($root_path . $dependency_file_data->filepath, $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
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
    drupal_goto('lab_migration_run');
  }
}

function lab_migration_download_lab()
{
  $lab_id = arg(3);
  $root_path = lab_migration_path();

  /* get solution data */
  $lab_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $lab_id);
  $lab_data = db_fetch_object($lab_q);
  $LAB_PATH = $lab_data->lab_title . '/';

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

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
        $zip->addFile($root_path . $solution_files_row->filepath, $LAB_PATH . $EXP_PATH . $CODE_PATH . $solution_files_row->filename);
      }
      /* dependency files */
      while ($solution_dependency_files_row = db_fetch_object($solution_dependency_files_q))
      {
        $dependency_file_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id));
        if ($dependency_file_data)
          $zip->addFile($root_path . $dependency_file_data->filepath, $LAB_PATH . $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . $dependency_file_data->filename);
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
    drupal_set_message("There are no solutions in this Lab to download", 'error');
    drupal_goto('lab_migration_run');
  }
}

