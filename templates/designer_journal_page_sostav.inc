<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/sostav'.
 */
function designer_journal_page_sostav($object_id) {
    // Проверяем, что пользователь авторизован и имеет необходимые права (если нужно)
    if (!user_access('access content')) {
        drupal_access_denied();
        return;
    }

    // Подключаем содержимое HTML-файла sec_header1.html
    $sec_header_path = drupal_get_path('module', 'designer_journal_page') . '/templates/sec_header2.html';
    $sec_header = file_get_contents($sec_header_path);

    // Заменяем {{object_id}} на реальное значение
    $sec_header = str_replace('{{object_id}}', '' . $object_id, $sec_header);
    $output = $sec_header;
    // Загружаем объект по object_id
    $object_node = node_load($object_id);
    //загружаем журнал от объекта
    $journal_id = $object_node->field_journal_nadzora_ref['und'][0]['target_id'];
    $journal_node = node_load($journal_id);

    if (!$journal_node || empty($journal_node->field_sostav_rbochei)) {
        return t('No data available.');
    }

    // Start building the table
    $header = array(
        t('ФИО'),
        t('Должность, место работы'),
        t('Работа, по которой осуществяется авторский надзор'),
        t('Дата и номер приказа о назначении'),
        t('Действия с записью')
    );

    $rows = array();

    // Loop through the field collection items
    foreach ($journal_node->field_sostav_rbochei['und'] as $item) {
        $field_collection_item = entity_load('field_collection_item', array($item['value']));
        if ($field_collection_item) {
            $item = reset($field_collection_item);

            $rows[] = array(
                isset($item->field_fio['und'][0]['value']) ? check_plain($item->field_fio['und'][0]['value']) : '',
                isset($item->field_dolzhnost['und'][0]['value']) ? check_plain($item->field_dolzhnost['und'][0]['value']) : '',
                isset($item->field_jobname['und'][0]['value']) ? check_plain($item->field_jobname['und'][0]['value']) : '',
                isset($item->field_datenum['und'][0]['value']) ? check_plain($item->field_datenum['und'][0]['value']) : '',
                t('удалить'),
            );
        }
    }










    // Выводим содержимое страницы
    $output .= '<style>
    .form-item {
        margin: 0 !important;
        padding: 0 !important;
        flex: 1 !important; /* Элементы будут занимать пространство в зависимости от установленного flex-basis */
    }

    .form-item-field-fio {
        flex: 0 0 15% !important; /* Ширина для field_fio */
    }

    .form-item-field-dolzhnost {
        flex: 0 0 15% !important; /* Ширина для field_dolzhnost */
    }

    .form-item-field-jobname {
        flex: 0 0 30% !important; /* Ширина для field_jobname */
    }

    .form-item-field-datenum {
        flex: 0 0 20% !important; /* Ширина для field_datenum */
    }

    .form-item input {
        width: 100% !important; /* Поле ввода занимает всю ширину своего контейнера */
        box-sizing: border-box !important; /* Учитываем padding и border в ширину элемента */
        margin-right: 0 !important; /* Убираем отступы справа */
    }

    .container {
        display: flex !important; /* Используем flexbox для расположения дочерних элементов */
    }

     
    
    
</style>';




    $output .= '<h1>' . t('Состав рабочей группы') . '</h1>';
    $output .= theme('table', array('header' => $header, 'rows' => $rows)); // рендер таблицы

    $output .= '<h1>' . t('Добавление в состав') . '</h1>';

    $form = drupal_get_form('designer_journal_sostav_rbochei_form', $journal_node);
    $output .= drupal_render($form);


    return $output;
}



//ФОРМА ДОБАВЛЕНИЯ


function designer_journal_sostav_rbochei_form($form, &$form_state, $journal_node) {
    // Поля формы и кнопка submit
    $form['fields_container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex;  align-items: center;  gap: 20px', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );

    $form['fields_container']['field_fio'] = array(
        '#type' => 'textfield',
        '#title' => 'ФИО',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['fields_container']['field_dolzhnost'] = array(
        '#type' => 'textfield',
        '#title' => 'Должность, место работы',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['fields_container']['field_jobname'] = array(
        '#type' => 'textfield',
        '#title' => 'Работа, по которой осуществляется надзор',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['fields_container']['field_datenum'] = array(
        '#type' => 'textfield',
        '#title' => 'Дата и номер приказа',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px; ',
        ),
    );

    // Кнопка для сохранения формы
    $form['fields_container']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Добавить'),
        '#attributes' => array(
            'class' => array('form-submit-inline'),
            'style' => 'align-self: center !important; margin: 0 !important', // Кнопка занимает 20% ширины контейнера
        ),
    );

    // Передаем узел журнала в форму
    $form_state['journal_node'] = $journal_node;

    return $form;
}





function designer_journal_sostav_rbochei_form_submit($form, &$form_state) {
    // Получаем ноду журнала
    $journal_node = $form_state['journal_node'];

    // Создаем новый элемент в коллекции
    $field_collection_item = entity_create('field_collection_item', array('field_name' => 'field_sostav_rbochei'));

    // Устанавливаем значения для полей
    $field_collection_item->setHostEntity('node', $journal_node);
    $field_collection_item->field_fio['und'][0]['value'] = $form_state['values']['field_fio'];
    $field_collection_item->field_dolzhnost['und'][0]['value'] = $form_state['values']['field_dolzhnost'];
    $field_collection_item->field_jobname['und'][0]['value'] = $form_state['values']['field_jobname'];
    $field_collection_item->field_datenum['und'][0]['value'] = $form_state['values']['field_datenum'];

    // Сохраняем элемент коллекции
    $field_collection_item->save();
    drupal_set_message('Успешно добавлено!');
    // Перезагружаем страницу после сохранения
    $form_state['redirect'] = $_GET['q'];
}


