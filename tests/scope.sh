function dotest {
    typeset -a out
    match '(\d+)' a960b out
    [[ -v out[0] ]] && [[  -v out[1] ]] && [[ ! -v out[2] ]]
}

function test {
    # should not override outside scope
    dotest && [[ ! -v out ]]
}
