function test {
    typeset pat='^.$'
    typeset str="你"
    ! match "$pat" "$str" && match -u "$pat" "$str"
}
