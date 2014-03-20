typeset line_filled="......................................................"
divert(-1)
changequote([[[,]]])
define([[[BEGIN_TEST]]], [[[((total++))
typeset  line_message="$1"
printf  "test ${line_message} ${line_filled:${#line_message}}"]]])dnl

define([[[END_TEST]]], [[[if (( $? == 0 )); then
    echo [$((++count))] $'\e[1;32m' ok $'\e[0m'
else
    echo  [${total}] $'\e[1;31m' fail $'\e[0m'
fi]]])
divert[[[]]]dnl
