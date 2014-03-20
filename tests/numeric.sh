function test {
    # 0(00) - 255
    for ((i=0; i < 256; i++)); do
        if ! match '^([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$' $i; then
            return 1
        fi
    done
}
