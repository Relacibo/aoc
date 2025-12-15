use std::{
    collections::HashMap,
    fs::File,
    io::{BufRead, BufReader},
    path::Path,
};

use const_format::concatcp;

const DEBUG_SOLUTION1: bool = false;
const DEBUG_SOLUTION2: bool = false;

const DAY: &str = "07";
const INPUT_FILE_NAME: &str = "input";
// const INPUT_FILE_NAME: &str = "example";
const INPUT_FILE_PATH: &str = concatcp!(
    env!("CARGO_MANIFEST_DIR"),
    "/resources/",
    DAY,
    "/",
    INPUT_FILE_NAME
);

pub fn main() {
    let res1 = solution1();
    println!("Solution 1: {res1}");
    let res2 = solution2();
    println!("Solution 2: {res2}");
}

fn solution1() -> i128 {
    let CharGrid { mut content } = parse_input();
    let mut res: i128 = 0;
    for r in 0..content.len() {
        let occs = content[r]
            .iter()
            .enumerate()
            .filter_map(|ch| (*ch.1 == 'S').then_some(ch.0))
            .collect::<Vec<_>>();
        if let Some(next_row) = content.get_mut(r + 1) {
            for i in occs {
                let indices = if next_row[i] == '^' {
                    res += 1;
                    [i.checked_sub(1), Some(i + 1)]
                } else {
                    [None, Some(i)]
                };
                let xs = indices.iter().flatten();
                for x in xs {
                    if let Some(e) = next_row.get_mut(*x) {
                        *e = 'S'
                    }
                }
            }
            if DEBUG_SOLUTION1 {
                println!("{:?}", content[r]);
            }
        }
    }
    res
}

#[derive(Hash, PartialEq, Eq, PartialOrd, Ord, Clone, Debug)]
struct Elem {
    row_num: usize,
    col_num: usize,
}

fn solution2() -> i128 {
    let CharGrid { mut content } = parse_input();

    let mut memo: HashMap<Elem, i128> = HashMap::new();
    let mut results: Vec<(Elem, i128)> = Vec::new();

    let mut stack: Vec<Elem> = Vec::new();
    let start = content[0].iter().position(|ch| *ch == 'S').unwrap();
    stack.push(Elem {
        row_num: 0,
        col_num: start,
    });

    while let Some(elem) = stack.pop() {
        let Elem { row_num, col_num } = elem;

        // Memo
        while results.len() > row_num {
            let sr_index = results.len() - 1;
            let (elem, elem_res) = results.pop().unwrap();
            memo.insert(elem, elem_res);
            if sr_index > 0
                && let Some((_, parent_res)) = &mut results.get_mut(sr_index - 1)
            {
                *parent_res += elem_res;
            }
        }

        let mut current_res = 0;
        let Some(next_row) = content.get_mut(row_num + 1) else {
            if DEBUG_SOLUTION2 {
                println!("Last row!");
            }
            continue;
        };
        let indices = if next_row[col_num] == '^' {
            [col_num.checked_sub(1), Some(col_num + 1)]
        } else {
            [None, Some(col_num)]
        };
        let xs = indices
            .iter()
            .flatten()
            .filter(|x| next_row.get(**x).is_some());
        let mut count = 0;
        for x in xs {
            let elem = Elem {
                row_num: row_num + 1,
                col_num: *x,
            };
            if let Some(x) = memo.get(&elem) {
                current_res += x;
            } else {
                stack.push(elem);
            }
            count += 1;
        }
        if count == 2 {
            current_res += 1;
        }
        assert_eq!(results.len(), row_num);
        if DEBUG_SOLUTION2 {
            dbg!(&elem);
        }
        results.push((elem, current_res));
        // dbg!(&results);
    }
    // Memo
    while results.len() > 1 {
        let sr_index = results.len() - 1;
        let (_, elem_res) = results.pop().unwrap();
        if sr_index > 0
            && let Some((_, parent_res)) = &mut results.get_mut(sr_index - 1)
        {
            *parent_res += elem_res;
        }
    }
    results[0].1 + 1
}

#[derive(Clone, Debug)]
struct CharGrid {
    content: Vec<Vec<char>>,
}

fn parse_input() -> CharGrid {
    println!("Parsing file: {INPUT_FILE_PATH}");
    let file_path = Path::new(INPUT_FILE_PATH);
    let file = File::open(file_path).unwrap();
    let content = BufReader::new(file)
        .lines()
        .map_while(Result::ok)
        .filter(|l| !l.is_empty())
        .map(|line| line.chars().collect())
        .collect::<Vec<_>>();
    CharGrid { content }
}
