# Аннотация к проекту

Проект создавался для решения одной единсвенной проблемы. Как быстро и удобно собраться группе
людей с самыми разными личными графиками?

Суть в том, что люди вносят свой график создают группы и графики участников сравниваются 
и все пользователи видят промежутки времени когда они все одновременно свободны. Не нужно вызванивать, дергать друг друга, назначать организатора.
Теперь достаточно просто внести свой график добавить друга и посмотреть когда можете пойти потусить вместе))))

Также удобно и в коммерческом направлении использовать. Условно есть компания с 20 сотрудниками. 
Они создают группу и компании будет удобнее подобрать  оптимальное время для своего персонала.

Большинство персональных планировщик проигрывают нам, потому что они персональные)) Суть в том, что там
куча сверх удобного редактирования своих ивентов, но дальше профита в 15 минут от этого редактирования выиграть в большинстве случаев.
А выяснить когда кучу разных непонятных типов можно собрать посложнее задачка)))

Если помечтать, то можно вкинуть еще карту и рекомендовать всякие места, где деньги на досуг можно потратить, условно боулинг или кафе
повышая удобство своего продукта для пользователя и хватая плату за рекламу с этих самых боулингов))))))

Проект MeetUp состоит из 3 частей:

1. Клиент на QT.

2. Асинхронный http сервер.

3. База данных на PostgreSQL.


# Реализация клиента

### Многооконность

Для реализации нужно было определиться с тем, как будет осуществляться смена контента в окне. Сначала были созданы  разные окна на каждую задачу, но такой варинат никуда не годится.
Поэтому, был написан навигатор, который умеет заменять экраны внутри QStackedWidget и предоставляет различные сценарии переходов от одного экрана к другому. Также необходимо знать о предыдущих экранах для возможности вернуться к ним и как-то передавать данные.

На рисунке 1 представлена схема приложения и переходы. Когда приложение запускается, мы видим заставку, потом выполняется замена экрана на стартовый. Далее осуществляется смена экранов при нажатии кнопок.
<p align="center">
  <img src="https://sun9-28.userapi.com/impg/WanJ2WfjRoESrmF5WW_NbpLI0Ewv5h7cYTiAIw/OljQnaamNaE.jpg?size=576x407&quality=96&sign=259b17d33fcac51f2a4e3f9b19c94657&type=album" />
</br>
Рис. 1
</p>

Подробнее про реализацию взаимодействия экранов: https://habr.com/ru/post/550440/

А далее каждое окно было заполнено нужными слоями с виджетами. Которые в свою очередь соединены сигналами и слотами. И были реализованы функции обработок сигналов.

### Отправка Http-запроса

Для отправки http запроса и получения ответа используется QNetworkAccessManager. 
И тут были потрачено много дней, чтобы разобраться в проблеме соединения Клиента с сервером.
Сервер принимает запрос, считывает заголовки, но тело запроса не видит, будто оно пустое.

Причем при отправке запроса с Postman или QUrl на наш сервер - все работает.

Причем при отправке  запроса с Qt на ехо сервер в интернете - все работает.

Но при связке Qt-клиент  -  наш сервер тело запроса куда то исчезает.

Примеры всех 3 взамимодествий: https://drive.google.com/file/d/1GC6AZUinCqThovLR4lYgEgC2eIcEkhTP/view?usp=sharing

Так как до защиты оставалось пару дней, а на решение этой проблемы было уделено ооочень много времени. Совместными усилиями пришли к пока что единственному выходу из ситуации: отправка JSON в заголовке запроса.
Создается заголовок с названием "JSON_DATA", а далее в его значение записывается JSON.

### Обработка Http-ответа

Ответ приходит в виде JSON. Далее генерирутся слои/виджеты, куда записываются данные с распарсенного JSON.


### Косяки

* Связь Qt-сервер
* Много кода, малло комментариев
* Копипаста при создании слоев
* не responsive design


### Ссылки на демнострацию работы клиента:

* [Meet_up v.3 (15.01.22)](https://www.youtube.com/watch?v=YraDpnZb7vk)
* [Meet_up v.2](https://www.youtube.com/watch?v=1rmKo4pDfnI)
* [Meet_up v.1](https://www.youtube.com/watch?v=sgDWmKSD5C8)
* [Meet_up v.0](https://youtu.be/VJB7_AQeRMI)


# Реализация сервера
Принимает POST/ с JSON в теле запроса.

JSON типа {“type_request” : {“.” : “....”, ....}}

Примеры в файле TEST.

Request: add_event: {"add_event":[{"user_id":"56","event_name":"breakfast","event_date":"01-06-2000", "description":"2132", "time_begin":"15:45", "time_end":"16:00"}]}

Response: {"add_event":{"event_id":"56"}}

Далее передается в карту обработки. Если такой запрос существует передает соответствующему парсеру
для конвертации в нужную структуру. Будь то ивент, персональные данные, или данные по группам, контактам.
Далее данные передаются для обработки. (Занесение в базу, поиск в базе)
Затем конвертируются обратно в строку и высылаются клиенту.


### Порядок работы:
1. Сервер получает запрос. 
2. Чекает его.
3. Достает тело запроса.
4. Передает в Route.
5. Route вычлеяет тип запроса и смотрит есть ли в карте такой обработчик.
6. Находит нужное звено по карте (project/route/src/RouteImpl.hpp строка 103-105)
7. Передает в соответствующий парсер (лежат в project/parser_class/JSON/*) этого звена, формирует нужную структуру, будь то группа или контакты и т.д..

Поля несущих структур можно посмотреть в project/parser_object/include/Structs.hpp

8. Записывает эти стурктуры в большой объект (ParserObject, лежит в project/parser_object/include/ParserObject.hpp), который умеет впитывать их в себя, его задача исключительно переносная между парсерами и обработчиками.
9. Передает этот крупный объект в обработчик а обработчик уже знает нужные ему поля, чаще всего строку в стуктуре вложенную в этот ParserObject.
10. Метод обработчик уже юзает методы коннекторы к базе(Select, Update, Delete, Select + Join), чтобы создать ивент или удалить группу и так далее.
   
Методы коннекторы прописаны в project/database_connector/PorstgreSQL/*

11. Отдает успех, неуспех, в случае последнего где и что вызвало проблемы. В некоторых ответах отдает ID
новой сущности или каскаду всех однотипных сущностей закрепленных за 1 пользователем.
12. Вписывает ответ в ParserObject.
13. ParserObject в парсер.
14. Строку из парсера клиенту.

Карту запросов можно глянуть в project/route/include/impl/RouteMapKeys.hpp
Все звенья обработки можно глянуть в project/route/src/RouteImpl.hpp

Карту БД можно воссоздать из файла project/database_connector/SQL/pool.sql

### Каркас сервера:

Асинхронный механизм (1 реальный, куча виртуальных потоков). 
Созданы асинхронные аналоги чтения и записи из файла, асинхронный потоки на их основе,
цикл событий контролирующий работу сервера. Также модули реализующие слушающий сокет и client acceptor.

### Косяки:

Некорректное тело запроса валит сервер на парсинге (не успели обработать).

Не использовали механизм исключений т.к. мало известная область. Руководстовались принципом, 
что если проблему можно отловить if'ом, то надо его юзать, а не
особый и вместе с тем медленный механизм исключений.

Стилистика не выдерживает единного стиля, работаем над этим.

### Важно:

Переписать костяк сервера на boost'e. 

# База данных

### Схема

На рисунке 2 представлена схема базы данных.
![изображение](https://user-images.githubusercontent.com/88785411/151950671-2438e502-114e-4150-a825-ba3c3fcb01d1.png)
Рис. 2
</p>

# Сборка
```bash
make build
```

#Запуск
```bash
make launch
```

#Необходиые библиотеки
```bash
1. nlohmann/json (есть в проекте).
2. libpq (ключ для компилятора -lpq) нужно предварительно установить в системе.
Команда: sudo pacman -S postgresql (для оболочки Manjaro).
```
