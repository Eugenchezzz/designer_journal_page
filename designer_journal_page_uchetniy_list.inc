<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/uchetniy_list'.
 */
/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/uchetniy_list'.
 */
function designer_journal_page_uchetniy_list($object_id) {
    // Проверяем, что пользователь авторизован и имеет необходимые права (если нужно)
    if (!user_access('access content')) {
        drupal_access_denied();
        return;
    }

    // Подключаем содержимое HTML-файла sec_header1.html
    $sec_header_path = drupal_get_path('module', 'designer_journal_page') . '/templates/sec_header4.html';
    $sec_header = file_get_contents($sec_header_path);

    // Заменяем {{object_id}} на реальное значение
    $sec_header = str_replace('{{object_id}}', '' . $object_id, $sec_header);
    $output = $sec_header;

    // Загружаем объект по object_id
    $object_node = node_load($object_id);

    // Загружаем журнал от объекта
    $journal_id = $object_node->field_journal_nadzora_ref['und'][0]['target_id'];
    $journal_node = node_load($journal_id);

    // Выводим содержимое страницы
    $output .= '<h1>' . t('Учетный лист') . '</h1>';

    // Формируем заголовок таблицы
    $header = array(
        t('Дата'),
        t('Нарушения'),
        t('Указания'),
        t('Запись произвел'),
        t('Ознакомлены (Организация)'),
        t('Ознакомлены (Заказчик)'),
        t('Отметка об исполнении (Производитель)'),
        t('Отметка об исполнении (Представитель)'),
    );

    $rows = array();

    // Проходимся по элементам коллекции полей
    foreach ($journal_node->field_uchetniy_list['und'] as $item) {
        $field_collection_item = entity_load('field_collection_item', array($item['value']));
        if ($field_collection_item) {
            $item = reset($field_collection_item);

            // Формируем строку для таблицы
            $rows[] = array(
                isset($item->field_list_date['und'][0]['value']) ? check_plain($item->field_list_date['und'][0]['value']) : '',
                isset($item->field_list_narushenija['und'][0]['value']) ? check_plain($item->field_list_narushenija['und'][0]['value']) : '',
                isset($item->field_list_ukazanija['und'][0]['value']) ? check_plain($item->field_list_ukazanija['und'][0]['value']) : '',
                isset($item->field_list_zapis_proizvel['und'][0]['value']) ? check_plain($item->field_list_zapis_proizvel['und'][0]['value']) : '',
                isset($item->field_list_oznak_org['und'][0]['value']) ? check_plain($item->field_list_oznak_org['und'][0]['value']) : '',
                isset($item->field_list_oznak_zak['und'][0]['value']) ? check_plain($item->field_list_oznak_zak['und'][0]['value']) : '',
                isset($item->field_list_otmetka_ukaz_proizv['und'][0]['value']) ? check_plain($item->field_list_otmetka_ukaz_proizv['und'][0]['value']) : '',
                isset($item->field_list_otmetka_ukaz_predst['und'][0]['value']) ? check_plain($item->field_list_otmetka_ukaz_predst['und'][0]['value']) : '',
            );
        }
    }

    // Рендерим таблицу
    $output .= theme('table', array('header' => $header, 'rows' => $rows));

    // Добавляем форму добавления новой записи
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

    .form-item-field-list-narushenija {
        flex: 0 0 10% !important; /* Ширина для field_jobname */
        width: 20%;

    }
    .form-item-field-list-ukazanija {
        flex: 0 0 10% !important; /* Ширина для field_jobname */
        width: 20%;

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
      $form = drupal_get_form('designer_journal_page_uchetniy_list_add_form', $journal_id);
      $output .= drupal_render($form);
    return $output;
}



// ФОРМА ДОБАВЛЕНИЯ И ЕЕ ОБРАБОТЧИК
/**
 * Форма для добавления новой записи в учетный лист.
 */
function designer_journal_page_uchetniy_list_add_form($form, &$form_state, $journal_id) {
// КОД ДЛЯ СОЗДАНИЯ МАССИВОВ ДОПУСТИМЫХ ДАННЫХ ДЛЯ ВЫБОРА
// Загружаем ноду
    $journal_node = node_load($journal_id);

    $fio_variants = [];
    $index = 0;

    if ($journal_node && isset($journal_node->field_predstaviteli_nadzora['und'])) {
        foreach ($journal_node->field_predstaviteli_nadzora['und'] as $item) {
            // Загружаем элемент коллекции полей
            $field_collection_item = entity_load('field_collection_item', array($item['value']));

            if ($field_collection_item) {
                $item = reset($field_collection_item);

                // Получаем значение ФИО
                $fio_value = isset($item->field_reg_fio['und'][0]['value']) ? $item->field_reg_fio['und'][0]['value'] : 'Unknown';

                // Проверяем, есть ли уже такое ФИО в массиве
                if (!in_array($fio_value, $fio_variants)) {
                    // Добавляем ФИО в массив, если оно не встречается
                    $fio_variants[$index] = $fio_value;
                    $index++;
                }
            }
        }



    } else {
        drupal_set_message(t('Node or field field_predstaviteli_nadzora not found.'));
    }










            // Сохраняем объект журнала в form_state для использования в обработчике
    $form_state['journal_node'] = node_load($journal_id);

    // Структура формы
    $form['field_container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex;  align-items: center;  gap: 20px', // Flex-wrap для переноса на новую строку при нехватке места
        ),

    );

    $form['field_container']['field_list_date'] = array(
        '#type' => 'date_popup',
        '#title' => t('Дата'),
        '#default_value' => !empty($field_collection_item->field_list_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($field_collection_item->field_list_date['und'][0]['value'])),
                'month' => date('n', strtotime($field_collection_item->field_list_date['und'][0]['value'])),
                'day' => date('j', strtotime($field_collection_item->field_list_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Обратите внимание на формат даты
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );


    $form['field_container']['field_list_narushenija'] = array(
        '#type' => 'textarea',
        '#title' => t('Нарушения'),
        '#required' => TRUE,
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),

    );

    $form['field_container']['field_list_ukazanija'] = array(
        '#type' => 'textarea',
        '#title' => t('Указания'),
        '#required' => TRUE,
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['field_container']['field_list_zapis_proizvel'] = array(
        '#type' => 'select',
        '#title' => t('Запись произвел'),
       '#options' => $fio_variants,
        '#required' => TRUE,
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    // Кнопка сабмита
    $form['field_container']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Добавить запись'),
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    return $form;
}



/**
 * Обработчик сабмита для формы добавления новой записи в учетный лист.
 */
function designer_journal_page_uchetniy_list_add_form_submit($form, &$form_state) {
    // Получаем ноду из состояния формы
    $journal_node = $form_state['journal_node'];

    // Проверяем, что нода существует
    if ($journal_node && isset($journal_node->nid)) {
        // Получаем данные из формы
        $date = isset($form_state['values']['field_list_date']) ? $form_state['values']['field_list_date'] : array();
        $violations = isset($form_state['values']['field_list_narushenija']) ? $form_state['values']['field_list_narushenija'] : '';
        $instructions = isset($form_state['values']['field_list_ukazanija']) ? $form_state['values']['field_list_ukazanija'] : '';
        $record_by = isset($form_state['values']['field_list_zapis_proizvel']) ? $form_state['values']['field_list_zapis_proizvel'] : '';

        // Преобразуем массив даты в строку формата Y-m-d
        $date_value = !empty($date['year']) && !empty($date['month']) && !empty($date['day']) ?
            sprintf('%04d-%02d-%02d', $date['year'], $date['month'], $date['day']) : '';

        // Создаем новый элемент коллекции полей для учетного листа
        $field_collection_item = entity_create('field_collection_item', array('field_name' => 'field_uchetniy_list'));
        $field_collection_item->setHostEntity('node', $journal_node);

        // Устанавливаем значения полей
        $field_collection_item->field_list_date['und'][0]['value'] = $date;
        $field_collection_item->field_list_narushenija['und'][0]['value'] = $violations;
        $field_collection_item->field_list_ukazanija['und'][0]['value'] = $instructions;
        $field_collection_item->field_list_zapis_proizvel['und'][0]['value'] = $record_by;

        // Сохраняем новый элемент коллекции полей
        $field_collection_item->save();
        drupal_set_message($date);
        drupal_set_message();
        drupal_set_message();
        drupal_set_message();
        // Перенаправляем пользователя на страницу журнала после добавления записи

        $form_state['redirect'] = $_SERVER['REQUEST_URI'];



    } else {
        form_set_error('', t('Нода журнала не найдена.'));
    }
}


