<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/sostav'.
 */
function designer_journal_page_nadzor_predstaviteli($object_id) {
  // Проверяем, что пользователь авторизован и имеет необходимые права (если нужно)
  if (!user_access('access content')) {
    drupal_access_denied();
    return;
  }

  // Подключаем содержимое HTML-файла sec_header1.html
  $sec_header_path = drupal_get_path('module', 'designer_journal_page') . '/templates/sec_header3.html';
  $sec_header = file_get_contents($sec_header_path);

  // Заменяем {{object_id}} на реальное значение
  $sec_header = str_replace('{{object_id}}', '' . $object_id, $sec_header);
  $output = $sec_header;
  // Загружаем объект по object_id
  $object_node = node_load($object_id);
  //загружаем журнал от объекта
  $journal_id = $object_node->field_journal_nadzora_ref['und'][0]['target_id'];
  $journal_node = node_load($journal_id);

  // Выводим содержимое страницы
  $output .= '<h1>' . t('Регистрация представителей авторского надзора') . '</h1>';



  //ВЫВОД ДАННЫХ В ТАБЛИЦЕ
  // Start building the table
  $header = array(
    t('Фамилия, инициалы'),
    t('Место работы'),
    t('Занимаемая должность'),
    t('Номер телефона'),
    t('Дата приезда'),
    t('Дата отъезда'),
    t('Действие'),
  );

  $rows = array();

  // цикл записи в табличку для отображения
    $index = 0;
  foreach ($journal_node->field_predstaviteli_nadzora['und'] as $item) {
    $field_collection_item = entity_load('field_collection_item', array($item['value']));
    if ($field_collection_item) {
      $item = reset($field_collection_item);


        // Форматируем дату из поля field_reg_start_date
        $start_date = isset($item->field_reg_start_date['und'][0]['value']) ? $item->field_reg_start_date['und'][0]['value'] : '';
        $formatted_start_date = $start_date ? date('Y-m-d', strtotime($start_date)) : '';

// Форматируем дату из поля field_reg_end_date
        $end_date = isset($item->field_reg_end_date['und'][0]['value']) ? $item->field_reg_end_date['und'][0]['value'] : '';
        $formatted_end_date = $end_date ? date('Y-m-d', strtotime($end_date)) : '';


      $rows[] = array(
          'data' => array(
           isset($item->field_reg_fio['und'][0]['value']) ? check_plain($item->field_reg_fio['und'][0]['value']) : '',
           isset($item->field_reg_place['und'][0]['value']) ? check_plain($item->field_reg_place['und'][0]['value']) : '',
           isset($item->field_reg_dolzhnost['und'][0]['value']) ? check_plain($item->field_reg_dolzhnost['und'][0]['value']) : '',
           isset($item->field_reg_phone['und'][0]['value']) ? check_plain($item->field_reg_phone['und'][0]['value']) : '',
           $formatted_start_date,
           $formatted_end_date,
           '<button class="edit-button" data-index="' . $index . '">' . t('Редактировать') . '</button>',
       ),
          'id' => 'row-' . $index,
      );

        $form = drupal_get_form('designer_journal_page_nadzor_edit_form', $item, $item->item_id);
        // Скрытая строка с формой
        $rows[] = array(
            'data' => array(
                array(
                    'data' => drupal_render($form),
                    'colspan' => 7, // Объединяем все колонки
                ),
            ),
            'class' => array('edit-form-row'),
            'id' => 'edit-row-' . $index,
            'style' => 'display: none;', // Скрываем форму по умолчанию
        );


    }
    $index++;
      drupal_add_js("
        jQuery(document).ready(function($) {
            $('.edit-button').click(function() {
                var index = $(this).data('index');
                $('#row-' + index).hide();
                $('#edit-row-' + index).show();
            });
        });
    ", 'inline');


  }

  // Render the table
  $output .= theme('table', array('header' => $header, 'rows' => $rows));

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
  $form = drupal_get_form('designer_journal_predstaviteli_nadzora_form', $journal_node);
  $output .= drupal_render($form);






  return $output;
}



// ФОРМА ДОБАВЛЕНИЯ ПРЕДСТАВИТЕЛЯ НАДЗОРА

/**
 * Функция для создания формы добавления данных в коллекцию полей field_predstaviteli_nadzora.
 */
function designer_journal_predstaviteli_nadzora_form($form, &$form_state, $journal_node) {

    $form['fields_container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex;  align-items: center;  gap: 20px', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );
  // Создаем форму с пустыми значениями по умолчанию
  $form['fields_container']['field_reg_fio'] = array(
    '#type' => 'textfield',
    '#title' => t('ФИО'),
    '#default_value' => '',
  );

  $form['fields_container']['field_reg_place'] = array(
    '#type' => 'textfield',
    '#title' => t('Место'),
    '#default_value' => '',
  );

  $form['fields_container']['field_reg_dolzhnost'] = array(
    '#type' => 'textfield',
    '#title' => t('Должность'),
    '#default_value' => '',
  );

  $form['fields_container']['field_reg_phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Телефон'),
    '#default_value' => '',
  );





    $form['fields_container']['field_reg_start_date'] = array(
        '#type' => 'date_popup',
        '#title' => t('Дата начала'),
        '#default_value' => !empty($field_collection_item->field_reg_start_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($field_collection_item->field_reg_start_date['und'][0]['value'])),
                'month' => date('n', strtotime($field_collection_item->field_reg_start_date['und'][0]['value'])),
                'day' => date('j', strtotime($field_collection_item->field_reg_start_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это должно убрать поле времени
    );

    $form['fields_container']['field_reg_end_date'] = array(
        '#type' => 'date_popup',
        '#title' => t('Дата окончания'),
        '#default_value' => !empty($field_collection_item->field_reg_end_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($field_collection_item->field_reg_end_date['und'][0]['value'])),
                'month' => date('n', strtotime($field_collection_item->field_reg_end_date['und'][0]['value'])),
                'day' => date('j', strtotime($field_collection_item->field_reg_end_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это должно убрать поле времени
    );









    $form['fields_container']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить'),
  );

  // Передаем узел журнала в форму
  $form_state['journal_node'] = $journal_node;

  return $form;
}


function designer_journal_predstaviteli_nadzora_form_submit($form, &$form_state) {
  // Получаем ноду журнала
  $journal_node = $form_state['journal_node'];

  // Проверяем, что нода журнала существует
  if (!$journal_node) {
    drupal_set_message(t('Не удалось найти журнал.'), 'error');
    return;
  }

  // Создаем новый элемент в коллекции
  $field_collection_item = entity_create('field_collection_item', array(
    'field_name' => 'field_predstaviteli_nadzora',
  ));




  // Устанавливаем значения для полей
  $field_collection_item->setHostEntity('node', $journal_node);
  $field_collection_item->field_reg_fio['und'][0]['value'] = $form_state['values']['field_reg_fio'];
  $field_collection_item->field_reg_place['und'][0]['value'] = $form_state['values']['field_reg_place'];
  $field_collection_item->field_reg_dolzhnost['und'][0]['value'] = $form_state['values']['field_reg_dolzhnost'];
  $field_collection_item->field_reg_phone['und'][0]['value'] = $form_state['values']['field_reg_phone'];

  //обработка дат
    // Получаем значения из формы
    $start_date = isset($form_state['values']['field_reg_start_date']) ? $form_state['values']['field_reg_start_date'] : '';
    $end_date = isset($form_state['values']['field_reg_end_date']) ? $form_state['values']['field_reg_end_date'] : '';

    // Проверяем наличие обеих дат
    if (empty($start_date) || empty($end_date)) {
        // Устанавливаем ошибку в форме
        if (empty($start_date)) {
            form_set_error('field_reg_start_date', t('Поле "Дата начала" обязательно для заполнения.'));
        }
        if (empty($end_date)) {
            form_set_error('field_reg_end_date', t('Поле "Дата окончания" обязательно для заполнения.'));
        }

        // Возвращаем пользователя на ту же страницу с формой
        $form_state['rebuild'] = TRUE;
    } else {
        // Если обе даты присутствуют, сохраняем данные
        $field_collection_item->field_reg_start_date['und'][0]['value'] = $start_date;
        $field_collection_item->field_reg_end_date['und'][0]['value'] = $end_date;

        // Вы можете выполнить дополнительные действия, если необходимо
        // Например, сохранение данных или редирект
    }





    // Сохраняем элемент коллекции
  $field_collection_item->save();



  // Отправляем сообщение об успешном добавлении
  drupal_set_message(t('Представитель успешно добавлен!'));

  // Перезагружаем текущую страницу
  $form_state['redirect'] = $_GET['q'];
}


// ОБРАБОТЧИКИ РЕДАКТИРОВАНИЯ ДАННЫХ




function designer_journal_page_nadzor_edit_form($form, &$form_state, $item, $item_id){
    $form['container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex; align-items: center; gap: 20px; flex-wrap: wrap;', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );

    $form['container']['field_reg_fio'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_fio['und'][0]['value']) ? $item->field_reg_fio['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_place'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_place['und'][0]['value']) ? $item->field_reg_place['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_dolzhnost'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_dolzhnost['und'][0]['value']) ? $item->field_reg_dolzhnost['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_phone'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_phone['und'][0]['value']) ? $item->field_reg_phone['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_start_date'] = array(
        '#type' => 'date_popup',

        '#default_value' => !empty($item->field_reg_start_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($item->field_reg_start_date['und'][0]['value'])),
                'month' => date('n', strtotime($item->field_reg_start_date['und'][0]['value'])),
                'day' => date('j', strtotime($item->field_reg_start_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это убирает поле времени
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_end_date'] = array(
        '#type' => 'date_popup',

        '#default_value' => !empty($item->field_reg_end_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($item->field_reg_end_date['und'][0]['value'])),
                'month' => date('n', strtotime($item->field_reg_end_date['und'][0]['value'])),
                'day' => date('j', strtotime($item->field_reg_end_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это убирает поле времени
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
            'class' => array('form-submit-inline', 'btn', 'btn-sm'),
            'style' => 'align-self: center !important; margin: 0 !important; padding: 5px 10px; font-size: 12px;', // Уменьшенная кнопка
        ),
    );

    return $form;
}

function designer_journal_page_nadzor_edit_form_submit($form, &$form_state)
{
    // Получаем значение item_id из скрытого поля
    $item_id = $form_state['values']['item_id'];

    // Загружаем элемент коллекции по item_id
    $field_collection_item = entity_load('field_collection_item', array($item_id));
    if ($field_collection_item) {
        $item = reset($field_collection_item);

        // Обновляем значения полей в элементе коллекции
        $item->field_reg_fio['und'][0]['value'] = $form_state['values']['field_reg_fio'];
        $item->field_reg_place['und'][0]['value'] = $form_state['values']['field_reg_place'];
        $item->field_reg_dolzhnost['und'][0]['value'] = $form_state['values']['field_reg_dolzhnost'];
        $item->field_reg_phone['und'][0]['value'] = $form_state['values']['field_reg_phone'];
        $item->field_reg_start_date['und'][0]['value'] = $form_state['values']['field_reg_start_date'];
        $item->field_reg_end_date['und'][0]['value'] = $form_state['values']['field_reg_end_date'];

        // Сохраняем изменения
        $item->save();
    }

    // Устанавливаем сообщение об успешном сохранении
    drupal_set_message(t('Данные успешно обновлены.'));

    // Перенаправляем на нужную страницу (например, на текущую страницу
}