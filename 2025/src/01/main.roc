app [main!] {
    pf: platform "https://github.com/roc-lang/basic-cli/releases/download/0.20.0/X73hGh05nNTkDHU06FHC0YfFaQB1pimX7gncRcao5mU.tar.br",
    unicode: "https://github.com/roc-lang/unicode/releases/download/0.3.0/9KKFsA4CdOz0JIOL7iBSI_2jGIXQ6TsFBXgd086idpY.tar.br",
}

import pf.Stdout
import pf.File
import unicode.Grapheme

Safe := { state : I32, reached_0_count : I32 }

default_safe : {} -> Safe
default_safe = |{}| @Safe({ state: 50, reached_0_count: 0 })

main! = |_args|
    path = "../../resources/01/input"

    input =
        File.read_utf8!(path)
        |> Result.with_default("")
        |> Str.split_on("\n")
        |> List.drop_if(Str.is_empty)
        |> List.map(parse_entry)

    @Safe({ reached_0_count }) =
        input
        |> List.walk(default_safe({}), turn)

    Stdout.line!("V1: ${Num.to_str reached_0_count}") ?? {}

    @Safe({ reached_0_count: reached_0_count2 }) =
        input
        |> List.walk(default_safe({}), turn2)

    Stdout.line!("V2: ${Num.to_str reached_0_count2}") ?? {}

    Ok({})

turn : Safe, I32 -> Safe
turn = |@Safe { state, reached_0_count }, amount|
    new_state = (state + amount) % 100
    new_reached_0_count = if state == 0 then reached_0_count + 1 else reached_0_count
    @Safe({ state: new_state, reached_0_count: new_reached_0_count })

turn2 : Safe, I32 -> Safe
turn2 = |@Safe { state, reached_0_count }, amount|
    temp_state = state + amount
    new_reached_0_count =
        if temp_state > 99 then
            reached_0_count + temp_state // 100
        else if temp_state < 0 then
            r = reached_0_count + ((-temp_state) // 100)
            if state == 0 then r else r + 1
        else if temp_state == 0 and state != 0 then
            reached_0_count + 1
        else
            reached_0_count
    @Safe({ state: (temp_state % 100 + 100) % 100, reached_0_count: new_reached_0_count })

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
