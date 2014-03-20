include(contri.m4)dnl
printf "THIS IS THE REGRESION TEST FOR MATCH\n\n"
builtin -f ./libmatch.so match

count=0
total=0

typeset -a ovector

for t in tests/*; do
    BEGIN_TEST([[["${t##tests/}"]]])
    source "$PWD/$t"
    test
    END_TEST
    unset -f test
done

BEGIN_TEST([[[case insensetive matching]]])
match -i Foo foo
END_TEST

BEGIN_TEST([[[basic match]]])
match  "\d+" 123abc ovector && [[ ${ovector[0]} == 123 ]]
END_TEST

BEGIN_TEST([[[matching url]]])
match  '^(http(s)?://)?www\.(.*)\.com$' 'www.google.com' ovector &&
    [[ ${ovector[3]} == google ]]
END_TEST


BEGIN_TEST([[[extended pattern expression]]])
match  -e  '# this is a test regexp
<html>(.+)</html> # match html top-level tags
' '<html>hello world</html>' ovector && [[ ${ovector[1]} == "hello world" ]]
END_TEST

# conclusion
printf  "\npass $count/$total\n"
