# Лабораторная работа №8

## Задание

К **suggest** сервису, который обрабатывает **POST** запросы из лабораторной работы №7, реализовать клиент.
Клиент отправляет **POST** запрос следующего формата:
```json
{
  "input": "hel"
}
```
В поле `input` находится часть текста, который клиент хочет ввести.

На данный запрос сервис должен предложить варианты того, что клиент возможно хочет ввести.</br>
Пример ответа:
```json
{
  "suggestions": [
    {
      "text": "hello",
      "position": 0
    },
    {
      "text": "hello world",
      "position": 1
    },
    {
      "text": "helm",
      "position": 2
    }
  ]
}
```
В случае если сервису нечего предложить он возвращает пустой массив `suggestions`.

Сервис варианты ответа считывает при инициализации и обновляет в ходе работы</br>
из конфигурационного файла **suggestions.json** в коллекцию `suggestions`. </br>
Обновление коллекции `suggestions` происходит каждые 15 минут из того же файла.</br>

Пример содержимого фала **suggestions.json**:
```json
[
  {
    "id": "hel",
    "name": "hello world",
    "cost": 70
  },
  {
    "id": "hel",
    "name": "hello",
    "cost": 10
  },
  {
    "id": "hel",
    "name": "helm",
    "cost": 200
  }
]
```

Для синхронизации доступа к коллекции `suggestions` стоит использовать `std::shared_mutex`.</br>
**Read** блокировка осуществляется в коде обработчика, а **Write** блокировка в функции обновления коллекции `suggestions`.</br>

Поле `cost` определяет значение поля `position`: чем больше `cost` тем меньше значение `position`.</br>
(чем ниже стоимость, тем выше вариант)

## Рекомендации

* для реализации HTTP сервера используйте готовые библиотеки
* в пакетном менеджере hunter есть несколько [библиотек](https://hunter.readthedocs.io/en/latest/packages/networking.html) для работы с сетью. Например, [Boost.Beast](https://hunter.readthedocs.io/en/latest/packages/pkg/Beast.html#pkg-beast)
* у хороших библиотек есть наглядные примеры работы с этими библиотеками. Например, простой [синхнорнный http сервер на основе Boost.Beast](https://github.com/boostorg/beast/blob/develop/example/http/server/sync/http_server_sync.cpp)
* тестирование сервера выполнить с помощью утилиты curl (документацию с примерами можно найти [здесь](https://ec.haxx.se/cmdline/cmdline-options)).
=======
В поле `input` находится текст, который предназначен для обработки сервисом. В случае, если сервису нечего предложить клиенту, то он вернет пустой массив.
Клиент обращается к сервису с именем пользователя (без пароля). Все соединения инициируются клиентом, где клиент запрашивает ответ от сервиса. Ответ от сервиса необходимо вывести с использованием стандартных потоков.

## Рекомендации

* для реализации HTTP клиента могут быть использованы готовые библиотеки
* в пакетном менеджере hunter есть несколько [библиотек](https://hunter.readthedocs.io/en/latest/packages/networking.html) для работы с сетью, например, [Boost.Beast](https://hunter.readthedocs.io/en/latest/packages/pkg/Beast.html#pkg-beast)
* у хороших библиотек есть наглядные примеры работы с этими библиотеками, например, простой [синхнорнный http клиент на основе Boost.Beast](https://github.com/boostorg/beast/blob/develop/example/http/client/sync/http_client_sync.cpp)

## Links

- [Boost.Beast](https://github.com/boostorg/beast)
- [Vinnie Falco: Boost.Beast](https://vinniefalco.github.io/beast/)
- [POST](https://ru.wikipedia.org/wiki/POST_(HTTP))
