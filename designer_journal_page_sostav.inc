<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/sostav'.
 */
function designer_journal_page_sostav($object_id) {
    $index = 0;
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



    $header = array(

        t('ФИО'),
        t('Должность, место работы'),
        t('Работа, по которой осуществляется авторский надзор'),
        t('Дата и номер приказа о назначении'),
        t('Действие'),
    );

    $rows = array();

    foreach ($journal_node->field_sostav_rbochei['und'] as $item) {
        $field_collection_item = entity_load('field_collection_item', array($item['value']));
        if ($field_collection_item) {
            $item = reset($field_collection_item);

            // Основная строка с данными
            $rows[] = array(
                'data' => array(

                    isset($item->field_fio['und'][0]['value']) ? check_plain($item->field_fio['und'][0]['value']) : '',
                    isset($item->field_dolzhnost['und'][0]['value']) ? check_plain($item->field_dolzhnost['und'][0]['value']) : '',
                    isset($item->field_jobname['und'][0]['value']) ? check_plain($item->field_jobname['und'][0]['value']) : '',
                    isset($item->field_datenum['und'][0]['value']) ? check_plain($item->field_datenum['und'][0]['value']) : '',
                    '<button class="edit-button" data-index="' . $index . '">' . t('Редактировать') . '</button>',
                ),
                'id' => 'row-' . $index,
            );
            $form = drupal_get_form('designer_journal_page_sostav_edit_form', $item, $item->item_id);
            // Скрытая строка с формой
            $rows[] = array(
                'data' => array(
                    array(
                        'data' => drupal_render($form),
                        'colspan' => 6, // Объединяем все колонки
                    ),
                ),
                'class' => array('edit-form-row'),
                'id' => 'edit-row-' . $index,
                'style' => 'display: none;', // Скрываем форму по умолчанию
            );
        }
        $index++;
    }

    // Добавляем скрипт для обработки кнопки "Редактировать"
    drupal_add_js("
        jQuery(document).ready(function($) {
            $('.edit-button').click(function() {
                var index = $(this).data('index');
                $('#row-' + index).hide();
                $('#edit-row-' + index).show();
            });
        });
    ", 'inline');










    // Выводим содержимое страницы
    $output .= '<style>

.edit-form-row td {
    padding: 0;
    margin: 0;
}

.edit-form-row form {
    width: 100%;
    display: flex;
    align-items: center;
}







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




















// ДИЧЬ ДЛЯ РЕДАКТИРОВАНИЯ ЗАПИСИ

function designer_journal_page_sostav_edit_form($form, &$form_state, $item, $item_id) {

    $form['container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex;  align-items: center;  gap: 20px', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );


    $form['container']['field_fio'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_fio['und'][0]['value']) ? $item->field_fio['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_dolzhnost'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_dolzhnost['und'][0]['value']) ? $item->field_dolzhnost['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_jobname'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_jobname['und'][0]['value']) ? $item->field_jobname['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_datenum'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_datenum['und'][0]['value']) ? $item->field_datenum['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['item_id'] = array(

        '#type' => 'hidden',
        '#default_value' => $item_id,
    );


    $form['container']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Сохранить изменения'),
        '#attributes' => array(
            'class' => array('form-submit-inline'),
            'style' => 'align-self: center !important; margin: 0 !important', // Кнопка занимает 20% ширины контейнера
        ),
    );





    return $form;
}

function designer_journal_page_sostav_edit_form_submit($form, &$form_state) {


    // Получаем данные из формы
    $item_id = $form_state['values']['item_id'];
    $field_datenum = $form_state['values']['field_datenum'];
    $field_dolzhnost = $form_state['values']['field_dolzhnost'];
    $field_jobname = $form_state['values']['field_jobname'];

    $field_fio = $form_state['values']['field_fio'];
    // Загружаем объект field_collection_item по ID
    $field_collection_item = entity_load('field_collection_item', array($item_id));
    if ($field_collection_item) {
        $item = reset($field_collection_item);
        $item->field_fio['und'][0]['value'] = $field_fio;
        $item->field_dolzhnost['und'][0]['value'] = $field_dolzhnost;
        $item->field_jobname['und'][0]['value'] = $field_jobname;
        $item->field_datenum['und'][0]['value'] = $field_datenum;

        // Сохраняем изменения
        field_attach_update('field_collection_item', $item);
        entity_save('field_collection_item', $item);
    }

    // Добавляем сообщение о успешном сохранении
    drupal_set_message(t('Изменения успешно сохранены.'));
    drupal_set_message(t($form_state['values']['item_id']));
    drupal_set_message(t($item_id));
    $form_state['redirect'] = $_SERVER['REQUEST_URI'];
}





