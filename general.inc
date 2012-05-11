<?php
// $Id$

function list_chapters()
{
  global $user;

  /************************ start approve book details ************************/
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('');
  }
  if ($proposal_data->proposal_status  != 1)
  {
    switch ($proposal_data->proposal_status )
    {
    case 0:
      drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
      drupal_goto('');
      return;
      break;
    case 2:
      drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
      drupal_goto('');
      return;
      break;
    case 3:
      drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'proposal') . '.'), 'status');
      drupal_goto('');
      return;
      break;
    default:
       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
       drupal_goto('');
       return;
       break;
    }
  }

  $preference_q = db_query("SELECT * FROM {lab_migration_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
  $preference_data = db_fetch_object($preference_q);
  if (!$preference_data)
  {
    drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
    drupal_goto('');
    return;
  }
  /************************ end approve book details **************************/

  $return_html = '<br />';
  $return_html .= '<strong>Title of the Book:</strong><br />' . $preference_data->book . '<br /><br />';
  $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->full_name . '<br /><br />';
  $return_html .= l('Upload Example Code', 'lab_migration/code/upload') . '<br />';

  /* get chapter list */
  $chapter_rows = array();
  $chapter_q = db_query("SELECT * FROM {lab_migration_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_data->id);
  while ($chapter_data = db_fetch_object($chapter_q))
  {
    /* get example list */
    $example_q = db_query("SELECT count(*) as example_count FROM {lab_migration_example} WHERE chapter_id = %d", $chapter_data->id);
    $example_data = db_fetch_object($example_q);
    $chapter_rows[] = array($chapter_data->number, $chapter_data->name . ' (' . l('Edit', 'lab_migration/code/chapter/edit/' . $chapter_data->id) . ')', $example_data->example_count, l('View', 'lab_migration/code/list_examples/' . $chapter_data->id));
  }

  /* check if there are any chapters */                                   
  if (!$chapter_rows)
  {
    drupal_set_message(t('No uploads found.'), 'status');
    return $return_html;
  }

  $chapter_header = array('Chapter No.', 'Title of the Chapter', 'Uploaded Examples', 'Actions');
  $return_html .= theme_table($chapter_header, $chapter_rows);
  return $return_html; 
}

function list_examples()
{
  global $user;

  /************************ start approve book details ************************/
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('');
  }
  if ($proposal_data->proposal_status  != 1)
  {
    switch ($proposal_data->proposal_status )
    {
    case 0:
      drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
      drupal_goto('');
      return;
      break;
    case 2:
      drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'proposal') . '.'), 'error');
      drupal_goto('');
      return;
      break;
    case 3:
      drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'proposal') . '.'), 'status');
      drupal_goto('');
      return;
      break;
    default:
       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
       drupal_goto('');
       return;
       break;
    }
  }

  $preference_q = db_query("SELECT * FROM {lab_migration_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
  $preference_data = db_fetch_object($preference_q);
  if (!$preference_data)
  {
    drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
    drupal_goto('');
    return;
  }
  /************************ end approve book details **************************/

  /* get chapter details */
  $chapter_id = arg(3);
  $chapter_q = db_query("SELECT * FROM {lab_migration_chapter} WHERE id = %d AND preference_id = %d LIMIT 1", $chapter_id, $preference_data->id);
  if ($chapter_data = db_fetch_object($chapter_q))
  {
    $return_html = '<br />';
    $return_html .= '<strong>Title of the Book:</strong><br />' . $preference_data->book . '<br /><br />';
    $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->full_name . '<br /><br />';
    $return_html .= '<strong>Chapter Number:</strong><br />' . $chapter_data->number . '<br /><br />';
    $return_html .= '<strong>Title of the Chapter:</strong><br />' . $chapter_data->name . '<br />';
  } else {
    drupal_set_message(t('Invalid chapter.'), 'error');
    drupal_goto('lab_migration/code');
    return;
  }

  $return_html .= '<br />' . l('Back to Chapter List', 'lab_migration/code');

  /* get example list */
  $example_rows = array();
  $example_q = db_query("SELECT * FROM {lab_migration_example} WHERE chapter_id = %d ORDER BY
    CAST(SUBSTRING_INDEX(number, '.', 1) AS BINARY) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', 2), '.', -1) AS UNSIGNED) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', -1), '.', 1) AS UNSIGNED) ASC", $chapter_id);
  while ($example_data = db_fetch_object($example_q))
  {
    /* approval status */
    $approval_status = '';
    switch ($example_data->approval_status)
    {
    case 0: $approval_status = 'Pending'; break;
    case 1: $approval_status = 'Approved'; break;
    case 2: $approval_status = 'Rejected'; break;
    }

    /* example files */
    $example_files = '';
    $example_files_q = db_query("SELECT * FROM {lab_migration_example_files} WHERE example_id = %d ORDER BY filetype", $example_data->id);
    while ($example_files_data = db_fetch_object($example_files_q))
    {
      $file_type = '';
      switch ($example_files_data->filetype)
      {
      case 'S': $file_type = 'Main or Source'; break;
      case 'R': $file_type = 'Result'; break;
      case 'X': $file_type = 'xcos'; break;
      default:
      }
      $example_files .= l($example_files_data->filename, 'download/file/' . $example_files_data->id) . ' (' . $file_type . ')<br />';
    }
    /* dependency files */
    $dependency_files = '';
    $dependency_files_q = db_query("SELECT dependency.id as dependency_id, dependency.filename as dependency_filename
      FROM {lab_migration_example_dependency} example_dependency JOIN {lab_migration_dependency_files} dependency
      ON example_dependency.dependency_id = dependency.id
      WHERE example_dependency.example_id = %d", $example_data->id);
    while ($dependency_files_data = db_fetch_object($dependency_files_q))
    {
      $dependency_files .= l($dependency_files_data->dependency_filename, 'download/dependency/' . $dependency_files_data->dependency_id) . ' (Dependency)<br />';
    }
    $example_files .= $dependency_files;

    if ($example_data->approval_status == 0)
    {
      $example_rows[] = array('data' => array($example_data->number, $example_data->caption, $approval_status, $example_files, l('Edit', 'lab_migration/code/edit/' . $example_data->id) . ' | ' . l('Delete', 'lab_migration/code/delete/' . $example_data->id, array('attributes' => array('onClick' => 'return confirm("Are you sure you want to delete the example?")')))), 'valign' => 'top');
    } else {
      $example_rows[] = array('data' => array($example_data->number, $example_data->caption, $approval_status, $example_files, l('Download', 'download/example/' . $example_data->id)), 'valign' => 'top');
    }
  }

  $example_header = array('Example No.', 'Caption', 'Status', 'Files', 'Action');
  $return_html .= theme_table($example_header, $example_rows);
  return $return_html;
}

function get_category_name($category_id) {
    $category_data = '';
  	switch ($category_id)
  	{
    case 0: $category_data = 'Not Selected'; break;
    case 1: $category_data = 'Fluid Mechanics'; break;
    case 2: $category_data = 'Control Theory & Control Systems'; break;
    case 3: $category_data = 'Chemical Engineering'; break;
    case 4: $category_data = 'Thermodynamics'; break;
    case 5: $category_data = 'Mechanical Engineering'; break;
    case 6: $category_data = 'Signal Processing'; break;
    case 7: $category_data = 'Digital Communications'; break;
    case 8: $category_data = 'Electrical Technology'; break;
    case 9: $category_data = 'Mathematics & Pure Science'; break;
    case 10: $category_data = 'Analog Electronics'; break;
    case 11: $category_data = 'Digital Electronics'; break;
    case 12: $category_data = 'Computer Programming'; break;
    case 13: $category_data = 'Others'; break;
    default: $category_data = 'Unknown'; break;
  	}
  	return $category_data;
}
