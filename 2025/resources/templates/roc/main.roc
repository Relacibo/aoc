app [main!] {
    pf: platform "https://github.com/roc-lang/basic-cli/releases/download/0.20.0/X73hGh05nNTkDHU06FHC0YfFaQB1pimX7gncRcao5mU.tar.br",
    # unicode: "https://github.com/roc-lang/unicode/releases/download/0.3.0/9KKFsA4CdOz0JIOL7iBSI_2jGIXQ6TsFBXgd086idpY.tar.br",
}

import pf.Stdout
import pf.File
# import unicode.Grapheme

day : Str
day = "00"

main! = |_args|
    path = "../../resources/${day}/input"

    input =
        File.read_utf8!(path)
        |> Result.with_default("")
        |> Str.split_on("\n")
        |> List.drop_if(Str.is_empty)
        |> List.map(parse_entry)

    res1 =
        input
        |> List.walk(default_safe({}), solution1)

    Stdout.line!("V1: ${Num.to_str reached_0_count}") ?? {}

    res2 =
        input
        |> List.walk(default_safe({}), solution2)

    Stdout.line!("V2: ${Num.to_str reached_0_count2}") ?? {}

    Ok({})

parse_entry : Str -> I32
parse_entry = |s|
    when Grapheme.split(s) ?? [] is
        [c, .. as r] ->
            rest = r |> Str.join_with("") |> Str.to_i32() |> Result.with_default(0)
            sign =
                when c is
                    "L" -> -1
                    "R" -> 1
                    _ -> crash "unreachable \"${s}\" (No L or R)"
            sign * rest

        _ -> crash "unreachable: \"${s}\" (Could not split graphemes)"
