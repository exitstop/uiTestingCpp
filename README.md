# uiClickerCpp
Скомпилировано с помощью Visual Studio 2019

Из настроек только прилинкована библиотека oleacc.lib
(Свойство проекта) (Компоновщик->Ввод->Дополнительные зависимости) Сюда добавлена 
oleacc.lib

# Cmake лучше не использовать, пока что не работает вывод wchar_t

Но если это не критично то можно запустить через батник: build_release.bat

Только в батнике нужно поставить для какой VS нужно сгенерить проект

-G"Visual Studio 16 2019"

-G"Visual Studio 15 2017 Win64"

-G"Visual Studio 14 2015 [arch]"

 Visual Studio 12 2013 [arch]

 Visual Studio 11 2012 [arch]

 Visual Studio 10 2010 [arch]

 Visual Studio 9 2008 [arch]

arch = Win64
