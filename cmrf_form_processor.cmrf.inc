<?php

function cmrf_form_processor_get_connector_name($profile=null) {
  return 'cmrf_form_processor_connector:'.$profile;
}

function cmrf_form_processor_get_connector_id($profile=null) {
  $connector_name = cmrf_form_processor_get_connector_name($profile);

  $core = cmrf_core_get_core();
  $connector_id = variable_get($connector_name);
  if (empty($connector_id)) {
    $connector_id = $core->registerConnector($connector_name, $profile);
    variable_set($connector_name, $connector_id);
  }
  return $connector_id;
}

function cmrf_form_processor_send_call($entity, $action, $parameters, $options, $profile=null) {
  $connector_id = cmrf_form_processor_get_connector_id($profile);
  $core = cmrf_core_get_core();
  $call = $core->createCall($connector_id, $entity, $action, $parameters, $options);
  $core->executeCall($call);
  return $call;
}

function cmrf_form_processor_queue_call($entity, $action, $parameters, $options, $profile=null) {
  $connector_id = cmrf_form_processor_get_connector_id($profile);
  $core = cmrf_core_get_core();
  $call = $core->createCall($connector_id, $entity, $action, $parameters, $options);
  $call->setScheduledDate(new \DateTime());
  $core->queueCall($call);
  return $call;
}

function cmrf_form_processor_get_processor_list($profile=null) {
  $processors = array();
  $parameters = array();
  $options['cache'] = '180 minutes';
  $call = cmrf_form_processor_send_call('FormProcessorInstance', 'list', $parameters, $options, $profile);
  $result = $call->getReply();
  foreach($result['values'] as $value) {
    $processors[$value['name']] = $value['title'];
  }
  return $processors;
}

function cmrf_form_processor_get_fields($form_processor, $profile=null, $additional_params=array()) {
  $fields = array();
  if (!is_array($additional_params)) {
    $additional_params = array();
  }
  $parameters = $additional_params + array('api_action' => $form_processor);
  $options = array();
  if (empty($additional_params)) {
    $options['cache'] = '180 minutes';
  }
  $call = cmrf_form_processor_send_call('FormProcessor', 'getfields', $parameters, $options, $profile);
  $result = $call->getReply();
  foreach($result['values'] as $name => $field) {
    $fields[$name] = $field;
  }
  return $fields;
}

function cmrf_form_processor_get_options($form_processor, $field_name, $profile=null, $additional_params=array()) {
  if (!is_array($additional_params)) {
    $additional_params = array();
  }
  $parameters = $additional_params + array('api_action' => $form_processor, 'field' => $field_name);
  $options = array();
  if (empty($additional_params)) {
    $options['cache'] = '180 minutes';
  }

  $call = cmrf_form_processor_send_call('FormProcessor', 'getoptions', $parameters, $options, $profile);
  $result = $call->getReply();
  if (isset($result['values'])) {
    return $result['values'];
  }
  return array();
}

function cmrf_form_processor_get_default_fields($form_processor, $profile=null) {
  $fields = array();
  $parameters = array('api_action' => $form_processor);
  $options['cache'] = '180 minutes';
  $call = cmrf_form_processor_send_call('FormProcessorDefaults', 'getfields', $parameters, $options, $profile);
  $result = $call->getReply();
  foreach($result['values'] as $name => $field) {
    $fields[$name] = $field;
  }
  return $fields;
}

function cmrf_form_processor_get_default_data($form_processor, $params, $cache='1 minute', $profile=null) {
  $data = array();
  $options = array();
  if (!empty($cache)) {
    $options['cache'] = $cache;
  }
  $call = cmrf_form_processor_send_call('FormProcessorDefaults', $form_processor, $params, $options, $profile);
  $result = $call->getReply();
  if ($call->getStatus() == \CMRF\Core\Call::STATUS_FAILED) {
    if (isset($result['error_message'])) {
      throw new \Exception(t('Something went wrong with retrieving default values for the form. Return error message is: @error', ['@error' => $result['error_message']]));
    } else {
      throw new \Exception(t('Something went wrong with retrieving default values for the form. No error message has been returned.'));
    }
  }
  foreach($result as $name => $value) {
    if ($name == 'is_error') {
      continue;
    }
    $data[$name] = $value;
  }
  return $data;
}
