use std::{
    collections::{HashMap, VecDeque},
    fs::File,
    io::{BufRead, BufReader},
    path::Path,
    str::FromStr,
};

use const_format::concatcp;

const DEBUG_SOLUTION1: bool = false;
const DEBUG_SOLUTION2: bool = false;

const DAY: &str = "07";
// const INPUT_FILE_NAME: &str = "input";
const INPUT_FILE_NAME: &str = "example";
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
    let mut res: i128 = 0;

    let mut memo: HashMap<Elem, i128> = HashMap::new();
    let mut results: Vec<HashMap<Elem, i128>> = Vec::new();

    let mut queue: Vec<Elem> = Vec::new();
    let start = content[0].iter().position(|ch| *ch == 'S').unwrap();
    queue.push(Elem {
        row_num: 0,
        col_num: start,
    });

    let mut last_row = 0;
    while let Some(elem) = queue.pop() {
        let Elem { row_num, col_num } = elem;
        let mut current_row_res = 0;
        let Some(next_row) = content.get_mut(row_num + 1) else {
            last_row = row_num;
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
        for x in xs {
            let elem = Elem {
                row_num: row_num + 1,
                col_num: *x,
            };
            if let Some(x) = memo.get(&elem) {
                res += x;
                current_row_res += 1;
            } else {
                queue.push(elem);
            }
        }

        // Memo
        if last_row > row_num {
            let nephews = results.pop().unwrap();
            nephews.values().sum();
            results.resize(row_num + 1, HashMap::new());
            results[row_num] += r;
            memo.insert(elem, r);
            if DEBUG_SOLUTION2 {
                println!("^^^^^^");
                dbg!(last_row);
                dbg!(row_num);
                dbg!(future_res);
                dbg!(current_row_res);
                dbg!(results[row_num]);
            }
        } else {
            results.resize(row_num + 1, 0);
            results[row_num] += current_row_res;
            if DEBUG_SOLUTION2 {
                println!(">>>>>>");
                dbg!(last_row);
                dbg!(row_num);
                dbg!(current_row_res);
                dbg!(results[row_num]);
            }
        }
        last_row = row_num;
    }
    res
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
