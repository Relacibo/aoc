app [main!] {
    pf: platform "https://github.com/roc-lang/basic-cli/releases/download/0.20.0/X73hGh05nNTkDHU06FHC0YfFaQB1pimX7gncRcao5mU.tar.br",
    maybe: "https://github.com/niclas-ahden/roc-maybe/releases/download/0.1.0/QbjfB928rAw71_NyBagk498PX-_VkgaF8CW1W4CqiHI.tar.br",
    # unicode: "https://github.com/roc-lang/unicode/releases/download/0.3.0/9KKFsA4CdOz0JIOL7iBSI_2jGIXQ6TsFBXgd086idpY.tar.br",
}

import pf.Stdout
import pf.File
import maybe.Maybe exposing [Maybe]
# import unicode.Grapheme

day : Str
day = "02"

main! = |_args|
    path = "../../resources/${day}/input"
    # path = "../../resources/${day}/example"

    input =
        File.read_utf8!(path)
        |> Result.with_default("")
        |> Str.split_on(",")
        |> List.drop_if(Str.is_empty)
        |> List.map(parse_entry)

    res1 =
        input
        |> List.map(solution1)
        |> List.sum()

    Stdout.line!("V1: ${Num.to_str res1}") ?? {}

    res2 =
        input
        |> List.map(solution2)
        |> List.sum()

    Stdout.line!("V2: ${Num.to_str res2}") ?? {}

    # (
    #     all_slices_equal(1010, 10, Nothing, 0)
    #     |> Inspect.to_str
    #     |> Stdout.line!
    # )
    # ?? {}
    Ok({})

solution1 : (U128, U128) -> U128
solution1 = |(s, e)|
    Stdout.line!("${Num.to_str(s)}-${Num.to_str(e)} Prefix: ${Num.to_str(first_half(s))}") ?? {}
    res = solution1_helper((s, e), first_half(s), 0)
    Stdout.line!("\nRes: ${Num.to_str(res)}") ?? {}
    res

solution1_helper : (U128, U128), U128, U128 -> U128
solution1_helper = |(s, e), curr_prefix, acc|
    curr = append_num(curr_prefix, curr_prefix)
    if curr > e then
        acc
    else
        _ = if curr >= s then
            Stdout.write!("${Num.to_str(curr)}, ") ?? {}
        else
            {}
        acc_new = if curr < s then acc else acc + curr
        solution1_helper((s, e), curr_prefix + 1, acc_new)

solution2 : (U128, U128) -> U128
solution2 = |(s, e)|
    Stdout.line!("${Num.to_str(s)}-${Num.to_str(e)}") ?? {}
    out =
        List.range({ start: At 1, end: At (num_digits(e) // 2) })
        |> List.join_map(|n| get_prefixes((s, e), n))
    # (
    #     out
    #     |> Inspect.to_str
    #     |> Stdout.line!
    # )
    # ?? {}
    # (out |> List.keep_if(is_silly) |> Inspect.to_str |> Stdout.line!) ?? {}
    (out |> List.drop_if(is_silly) |> Inspect.to_str |> Stdout.line!) ?? {}
    res =
        out
        |> List.drop_if(is_silly)
        |> List.map(
            |prefix| all_silly_nums((s, e), 0, prefix, prefix),
        )
        |> List.sum()
    Stdout.line!("\nRes: ${Num.to_str(res)}") ?? {}
    res

get_prefixes : (U128, U128), U32 -> List U128
get_prefixes = |(s, e), digits|
    s_digits = num_digits(s)
    e_digits = num_digits(e)
    # Stdout.line!("Prefix: (s=${Num.to_str(s)}, e=${Num.to_str(e)}), digits=${Num.to_str(digits)}, s_digits=${Num.to_str(s_digits)}, e_digits=${Num.to_str(e_digits)}") ?? {}
    if s_digits < digits then
        []
    else if e_digits == s_digits then
        mod = (Num.pow_int(10, s_digits - digits) |> Num.to_u128())
        s_prefix = s // mod
        e_prefix = e // mod
        res = List.range({ start: At s_prefix, end: At e_prefix })
        # (res |> Inspect.to_str |> Stdout.line!) ?? {}
        res
    else
        s_digits_diff = s_digits - digits
        s_first = s // (Num.pow_int(10, s_digits_diff) |> Num.to_u128())
        s_last = Num.pow_int(10, digits) |> Num.to_u128()
        e_digits_diff = e_digits - digits
        e_first = Num.pow_int(10, digits - 1) |> Num.to_u128()
        e_last = e // (Num.pow_int(10, e_digits_diff) |> Num.to_u128())
        if e_first >= s_first then
            List.range({ start: At e_first, end: Before s_last })
        else
            big = if digits > s_digits // 2 then
                []
            else
                List.range({ start: At s_first, end: Before s_last })
            # Stdout.line!("e_digits_diff=${Num.to_str(e_digits_diff)}, e_first=${Num.to_str(e_first)}, e_last=${Num.to_str(e_last)}") ?? {}
            res =
                List.range({ start: At e_first, end: At e_last })
                |> List.concat(big)
            # (res |> Inspect.to_str |> Stdout.line!) ?? {}
            res

is_silly : U128 -> Bool
is_silly = |num|
    digits = num_digits(num)
    if digits <= 1 then
        Bool.false
    else
        List.range({ start: At 1, end: At (digits // 2) })
        |> List.map(|pow| Num.pow_int(10, pow |> Num.to_u128()))
        |> List.any(
            |mod|
                when all_slices_equal(num, mod, Nothing, 0) is
                    Just count if count > 1 -> Bool.true
                    _ -> Bool.false,
        )

all_slices_equal : U128, U128, Maybe U128, U32 -> Maybe U32
all_slices_equal = |num, mod, pivot, acc|
    if num == 0 then
        Just acc
    else
        val = num % mod
        new_num = num // mod
        when pivot is
            Just p if val != p -> Nothing
            _ -> all_slices_equal(new_num, mod, Just val, acc + 1)

all_silly_nums : (U128, U128), U128, U128, U128 -> U128
all_silly_nums = |(s, e), acc, last, num|
    curr = append_num(last, num)
    # Stdout.line!("${Num.to_str(curr)}") ?? {}
    if curr > e then
        acc
    else
        acc_next = if curr < s then
            acc
        else
            (curr |> Inspect.to_str |> Stdout.write!) ?? {}
            Stdout.write!(", ") ?? {}
            acc + curr
        all_silly_nums((s, e), acc_next, curr, num)

append_num : U128, U128 -> U128
append_num = |base, num|
    (Num.pow_int(10, num_digits(num)) |> Num.to_u128()) * base + num

first_half : U128 -> U128
first_half = |num|
    digits = num_digits(num)
    new_digits = digits - digits // 2
    num // (Num.pow_int(10, new_digits) |> Num.to_u128())

num_digits : U128 -> U32
num_digits = |num|
    num_digits_helper(num, 0)

num_digits_helper : U128, U32 -> U32
num_digits_helper = |num, acc|
    if num < 10 then
        acc + 1
    else
        num_digits_helper(num // 10, acc + 1)

parse_entry : Str -> (U128, U128)
parse_entry = |st|
    when st |> Str.split_on("-") is
        [s, e] -> ((s |> Str.trim |> Str.to_u128) ?? 0, (e |> Str.trim |> Str.to_u128) ?? 0)
        _ -> crash "input parsing - Could not split pair"

