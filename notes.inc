<?php
// $Id$

/******************************************************************************/
/***************************** BOOK NOTES *************************************/
/******************************************************************************/

function lab_migration_lab_notes_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $proposal_id);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message(t('Invalid lab selected. Please try again.'), 'error');
    drupal_goto('lab_migration/code_approval');
    return;
  }

  /* get current notes */
  $notes = '';
  $notes_q = db_query("SELECT * FROM {lab_migration_notes} WHERE proposal_id = %d LIMIT 1", $proposal_id);
  if ($notes_q)
  {
    $notes_data = db_fetch_object($notes_q);
    $notes = $notes_data->notes;
  }

  $form['lab_details'] = array(
    '#type' => 'item',
    '#value' => '<span style="color: rgb(128, 0, 0);"><strong>About the Lab</strong></span><br />' .
      '<strong>Proposer:</strong> ' . $proposal_data->name . '<br />' .
      '<strong>Title of the Lab:</strong> ' . $proposal_data->lab_title . '<br />'
  );

  $form['notes'] = array(
    '#type' => 'textarea',
    '#rows' => 20,
    '#title' => t('Notes for Reviewers'),
    '#default_value' => $notes,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Back'), 'lab_migration/code_approval'),
  );
  return $form;
}

function lab_migration_lab_notes_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $proposal_id);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message(t('Invalid lab selected. Please try again.'), 'error');
    drupal_goto('lab_migration/code_approval');
    return;
  }

  /* find existing notes */
  $notes_q = db_query("SELECT * FROM {lab_migration_notes} WHERE proposal_id = %d LIMIT 1", $proposal_id);
  $notes_data = db_fetch_object($notes_q);

  /* add or update notes in database */
  if ($notes_data) {
    db_query("UPDATE {lab_migration_notes} SET notes = '%s' WHERE id = %d", $form_state['values']['notes'], $notes_data->id);
    drupal_set_message('Notes updated successfully.', 'status');
  } else {
    db_query("INSERT INTO {lab_migration_notes} (proposal_id, notes) VALUES (%d, '%s')", $proposal_id, $form_state['values']['notes']);
    drupal_set_message('Notes added successfully.', 'status');
  }
}

