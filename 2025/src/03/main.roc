app [main!] {
    pf: platform "https://github.com/roc-lang/basic-cli/releases/download/0.20.0/X73hGh05nNTkDHU06FHC0YfFaQB1pimX7gncRcao5mU.tar.br",
    unicode: "https://github.com/roc-lang/unicode/releases/download/0.3.0/9KKFsA4CdOz0JIOL7iBSI_2jGIXQ6TsFBXgd086idpY.tar.br",
}

import pf.Stdout
import pf.File
import unicode.Grapheme

day : Str
day = "03"

main! = |_args|
    path = "../../resources/${day}/input"
    # path = "../../resources/${day}/example"

    input =
        File.read_utf8!(path)
        |> Result.with_default("")
        |> Str.split_on("\n")
        |> List.drop_if(Str.is_empty)
        |> List.map(parse_entry)

    res1 =
        input
        |> List.map(solution1)
        |> List.sum

    Stdout.line!("V1: ${Num.to_str res1}") ?? {}

    res2 =
        input
        |> List.map(solution2)
        |> List.sum

    Stdout.line!("V2: ${Num.to_str res2}") ?? {}

    Ok({})

solution1 : List U32 -> U32
solution1 = |l|
    last_index = List.len(l) - 1
    (f, s) =
        l
        |> List.walk_with_index(
            (0, 0),
            |(first, second), x, index|
                if x > first and index != last_index then
                    (x, 0)
                else if x > second then
                    (first, x)
                else
                    (first, second),
        )
    res = f * 10 + s
    Stdout.line!("${Num.to_str(res)}") ?? {}
    res

solution2 : List U32 -> U64
solution2 = |l|
    len = List.len(l)
    res_list =
        l
        |> List.walk_with_index(
            List.repeat(0, 12),
            |list, x, index|
                insert(list, len - index - 1, x),
        )
    res = convert(res_list)
    Stdout.line!("${Num.to_str(res)}") ?? {}
    res

insert : List U32, U64, U32 -> List U32
insert = |l, rev_index, elem|
    len = 12
    when l |> List.map_with_index(|x, i| (len - i, x)) |> List.find_first_index(|(i, x)| elem > x and i <= (rev_index + 1)) is
        Ok(index) -> [List.take_first(l, index), [elem], List.repeat(0, len - index - 1)] |> List.join
        _ -> l

convert : List U32 -> U64
convert = |l|
    l |> List.walk(0, |acc, x| acc * 10 + (x |> Num.to_u64))

parse_entry : Str -> List U32
parse_entry = |s|
    when Grapheme.split(s) ?? [] is
        d -> d |> List.map(|i| i |> Str.to_u32 |> Result.with_default(0))
        _ -> crash "unreachable: \"${s}\" (Could not split graphemes)"
