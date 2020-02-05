include(CheckCXXSymbolExists)

# check for symbol and set HAVE_SYMBOL
function(check_decl DECL HEADER)
    safe_string(${DECL} DECL_CONST)
    check_cxx_symbol_exists(${DECL} ${HEADER} HAVE_${DECL_CONST})
endfunction(check_decl)
