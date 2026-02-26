# ➕ Plus Language Interpreter (C)

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)

Bu proje, C dili ile sıfırdan (from scratch) geliştirilmiş hafif ve özel bir programlama dili yorumlayıcısıdır (interpreter). Herhangi bir harici derleyici kütüphanesi (Lex/Yacc vb.) kullanılmadan; sözcük analizi (lexical analysis/tokenization), bellek/değişken yönetimi ve komut işletim süreçleri doğrudan kaynak kod üzerinden yürütülür.

## 🚀 Temel Özellikler

* **Kendi Lexer'ı (Sözcük Analizcisi):** Kaynak kodu okur ve `KEYWORD`, `IDENTIFIER`, `OPERATOR`, `INTCONST`, `STRINGCONST` gibi token'lara ayrıştırır.
* **Dinamik Değişken Yönetimi:** Çalışma zamanında (runtime) değişken tanımlama, değer atama ve aritmetik operatörler (`:=`, `+=`, `-=`).
* **Döngü Mekanizması:** İçiçe (nested) blokları destekleyen, sayılar veya değişkenler üzerinden çalışan `repeat ... times` döngü yapısı.
* **Hata Ayıklama (Error Handling):** Eksik noktalı virgül (`;`), kapatılmamış bloklar (`}`), tanımsız değişkenler veya bilinmeyen karakterler için satır numarası tabanlı spesifik hata fırlatma.

## 🧠 Nasıl Çalışır? (Under the Hood)

Yorumlayıcı, `.ppp` uzantılı kaynak kod dosyasını satır satır okuyarak iki temel aşamadan geçirir:
1. **Tokenization (`tokenizeLine`):** Boşluklar temizlenir; komutlar, operatörler ve değerler `Token` yapısı (struct) içerisinde sınıflandırılarak bir diziye eklenir.
2. **Interpretation (`interpretCommands`):** AST (Abstract Syntax Tree) oluşturmaya gerek kalmadan, oluşturulan token dizisi üzerinde doğrudan (direct execution) ilerlenir. Durum (State) yönetimi, basit ve hızlı bir değişken dizisi (`Variable vars[]`) üzerinden sağlanır.

## 📝 "Plus" Dili Sentaksı (Syntax)

Dilin tasarımı, okunabilirliği yüksek ve temel algoritmik işlemleri yapmaya uygun bir yapıdadır. Örnek bir `.ppp` kodu:

```text
number x;
x := 10;
x += 5;

write "X degerimiz: " and x newline;

number sayac;
sayac := 3;

repeat sayac times {
    write "Dongu calisiyor..." newline;
    x -= 1;
}

write "Son x degeri: " and x;
```

## ⚙️ Kurulum ve Derleme

Projeyi derlemek ve çalıştırmak için sisteminizde standart bir C derleyicisinin (GCC veya Clang) bulunması yeterlidir.

1. Repoyu klonlayın:
   ```bash
   git clone [https://github.com/AliEmreBulut/plus-language-interpreter-c.git](https://github.com/AliEmreBulut/plus-language-interpreter-c.git)
   cd plus-language-interpreter-c
   ```
2. Kaynak kodu derleyin:
   ```bash
   gcc main.c -o plus_interpreter
   ```
3. Bir `.ppp` dosyası oluşturun ve yorumlayıcıya verin:
   ```bash
   ./plus_interpreter test.ppp
   ```
   *(Not: Dosya belirtilmezse program varsayılan olarak dizindeki `test.ppp` dosyasını arayacaktır.)*
