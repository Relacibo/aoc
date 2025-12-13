use std::{
    fs::File,
    io::{BufRead, BufReader},
    path::Path,
    str::FromStr,
};

use const_format::concatcp;

const DEBUG_SOLUTION2: bool = false;

const DAY: &str = "06";
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
    let input = parse_input1();
    solve(&input)
}

fn solve(input: &[InputEntry]) -> i128 {
    input
        .iter()
        .map(|InputEntry { op, numbers }| match op {
            Operation::Prod => numbers.iter().product::<i128>(),
            Operation::Sum => numbers.iter().sum(),
        })
        .sum()
}

fn parse_input1() -> Vec<InputEntry> {
    println!("Parsing file: {INPUT_FILE_PATH}");
    let file_path = Path::new(INPUT_FILE_PATH);
    let file = File::open(file_path).unwrap();
    let input = BufReader::new(file)
        .lines()
        .map_while(Result::ok)
        .filter(|l| !l.is_empty())
        .map(|line| {
            line.split_ascii_whitespace()
                .map(ToOwned::to_owned)
                .collect::<Vec<_>>()
        })
        .collect::<Vec<_>>();
    // transpose
    (0..input[0].len())
        .map(|x| (0..input.len()).map(|y| &input[y][x]).collect::<Vec<_>>())
        .map(|l| match &l[..] {
            [nums @ .., op] => InputEntry {
                op: Operation::from_str(op).unwrap(),
                numbers: nums.iter().map(|s| str::parse(s).unwrap()).collect(),
            },
            _ => {
                panic!("Input not supported {l:?}");
            }
        })
        .collect()
}

fn solution2() -> i128 {
    println!("Parsing file: {INPUT_FILE_PATH}");
    let file_path = Path::new(INPUT_FILE_PATH);
    let file = File::open(file_path).unwrap();
    let input = BufReader::new(file)
        .lines()
        .map_while(Result::ok)
        .filter(|l| !l.is_empty())
        .collect::<Vec<_>>();
    let [nums @ .., ops] = &input[..] else {
        panic!("Empty input");
    };
    let ops = ops
        .split_ascii_whitespace()
        .map(|op| Operation::from_str(op).unwrap())
        .collect::<Vec<_>>();

    let rows = nums
        .iter()
        .map(|s| s.chars().collect::<Vec<_>>())
        .collect::<Vec<_>>();
    let mut res = 0;
    let mut op_num = 0;
    let mut registers = Vec::new();
    for c in 0..rows[0].len() {
        let mut all_spaces = true;
        let mut num = 0;
        for row in &rows {
            let ch = row[c];
            if ch != ' ' {
                all_spaces = false;
                let digit = ch.to_digit(10).unwrap();
                num = num * 10 + (digit as i128);
            }
        }
        if !all_spaces {
            if DEBUG_SOLUTION2 {
                dbg!(num);
            }
            registers.push(num);
        };
        if all_spaces || c == rows[0].len() - 1 {
            if DEBUG_SOLUTION2 {
                dbg!(ops[op_num]);
            }
            let res_row = match ops[op_num] {
                Operation::Prod => registers.iter().product::<i128>(),
                Operation::Sum => registers.iter().sum(),
            };
            if DEBUG_SOLUTION2 {
                dbg!(res_row);
            }
            res += res_row;
            op_num += 1;
            registers = Vec::new();
        }
    }
    res
}

#[derive(Clone, Copy, Debug)]
enum Operation {
    Prod,
    Sum,
}

impl FromStr for Operation {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let res = match s {
            "*" => Operation::Prod,
            "+" => Operation::Sum,
            _ => {
                return Err(());
            }
        };
        Ok(res)
    }
}

#[derive(Clone, Debug)]
struct InputEntry {
    op: Operation,
    numbers: Vec<i128>,
}
