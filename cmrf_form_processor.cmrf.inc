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

function cmrf_form_processor_get_fields($form_processor, $profile=null) {
  $fields = array();  
  $parameters = array('api_action' => $form_processor);
  $options['cache'] = '180 minutes';
  $call = cmrf_form_processor_send_call('FormProcessor', 'getfields', $parameters, $options, $profile);
  $result = $call->getReply();
  foreach($result['values'] as $name => $field) {
    $fields[$name] = $field;
  }
  return $fields;
}
