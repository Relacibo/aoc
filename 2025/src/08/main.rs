use std::{
    collections::HashSet,
    fmt::Display,
    fs::File,
    io::{BufRead, BufReader, Write},
    path::Path,
    process::Command,
};

use const_format::concatcp;
use itertools::Itertools;

const DEBUG_SOLUTION1: bool = false;
const DEBUG_SOLUTION2: bool = true;

const DAY: &str = "08";
const INPUT_FILE_NAME: &str = "input";
const SHORTEST_LEN: usize = 1000;
// const INPUT_FILE_NAME: &str = "example";
// const SHORTEST_LEN: usize = 10;
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
    wl_copy(&res2.to_string());
}

fn wl_copy(text: &str) {
    if let Ok(mut child) = Command::new("wl-copy")
        .stdin(std::process::Stdio::piped())
        .spawn()
    {
        if let Some(mut stdin) = child.stdin.take() {
            let _ = stdin.write_all(text.as_bytes());
        }

        let _ = child.wait();
    }
}

#[derive(Copy, Clone, Debug)]
struct Vec3 {
    x: i64,
    y: i64,
    z: i64,
}

impl Display for Vec3 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{},{},{}", self.x, self.y, self.z)
    }
}

impl Vec3 {
    fn distance_pow(&self, other: &Vec3) -> i128 {
        let Vec3 { x, y, z } = self;
        let Vec3 {
            x: x_other,
            y: y_other,
            z: z_other,
        } = other;
        ((x - x_other) as i128).pow(2)
            + ((y - y_other) as i128).pow(2)
            + ((z - z_other) as i128).pow(2)
    }
}

fn solution1() -> i128 {
    let input = parse_input();
    let mut shortest = Vec::<(usize, usize, i128)>::new();
    for element @ (_, _, distance) in input.iter().enumerate().combinations(2).map(|v| {
        let [(i_a, a), (i_b, b)] = &v[..] else {
            unreachable!()
        };
        (*i_a, *i_b, a.distance_pow(b))
    }) {
        let position = shortest
            .binary_search_by_key(&distance, |x| x.2)
            .or_else(Result::<usize, ()>::Ok)
            .unwrap();
        if position < SHORTEST_LEN {
            shortest.insert(position, element);
            shortest.truncate(SHORTEST_LEN);
        }
    }
    if DEBUG_SOLUTION1 {
        print!("Shortest: [");
        for (a, b, d) in &shortest {
            print!("(({}),({}),{d})", input[*a], input[*b]);
        }
        println!("]");
    }

    let mut groups = Vec::<HashSet<usize>>::new();
    for (a, b, _) in shortest {
        let a_in = groups.iter().position(|set| set.contains(&a));
        let b_in = groups.iter().position(|set| set.contains(&b));
        match (a_in, b_in) {
            (None, None) => groups.push([a, b].into()),
            (None, Some(b_in)) => {
                groups[b_in].insert(a);
            }
            (Some(a_in), None) => {
                groups[a_in].insert(b);
            }
            (Some(a_in), Some(b_in)) if a_in == b_in => {}
            (Some(a_in), Some(b_in)) => {
                let (index, set) = if a_in < b_in {
                    let b_set = groups.remove(b_in);
                    (a_in, b_set)
                } else {
                    let a_set = groups.remove(a_in);
                    (b_in, a_set)
                };
                for x in set {
                    groups[index].insert(x);
                }
            }
        }
    }
    if DEBUG_SOLUTION1 {
        for g in &groups {
            for i in g {
                print!("{};", input[*i]);
            }
            println!();
        }
    }
    groups.sort_by_key(|s| s.len());
    groups
        .iter()
        .rev()
        .take(3)
        .map(|h| h.len() as i128)
        .product()
}

fn prepare_input(input: &[Vec3], threshold: usize) -> Vec<(usize, usize)> {
    let mut sorted_by_distance = Vec::<(usize, usize)>::new();
    let mut distances = Vec::<i128>::new();
    for (i_a, i_b, distance) in input.iter().enumerate().combinations(2).map(|v| {
        let [(i_a, a), (i_b, b)] = &v[..] else {
            unreachable!()
        };
        (*i_a, *i_b, a.distance_pow(b))
    }) {
        let position = distances
            .binary_search(&distance)
            .or_else(Result::<usize, ()>::Ok)
            .unwrap();
        if position < threshold {
            sorted_by_distance.insert(position, (i_a, i_b));
            sorted_by_distance.truncate(threshold);
            distances.insert(position, distance);
            distances.truncate(threshold);
        }
    }
    sorted_by_distance
}

fn prepare_input_filtered<F>(input: &[Vec3], threshold: usize, predicate: F) -> Vec<(usize, usize)>
where
    F: Fn((usize, usize)) -> bool,
{
    let mut sorted_by_distance = Vec::<(usize, usize)>::new();
    let mut distances = Vec::<i128>::new();
    for (i_a, i_b, distance) in input.iter().enumerate().combinations(2).filter_map(|v| {
        let [(i_a, a), (i_b, b)] = &v[..] else {
            unreachable!()
        };
        predicate((*i_a, *i_b)).then_some((*i_a, *i_b, a.distance_pow(b)))
    }) {
        let position = distances
            .binary_search(&distance)
            .or_else(Result::<usize, ()>::Ok)
            .unwrap();
        if position < threshold {
            sorted_by_distance.insert(position, (i_a, i_b));
            sorted_by_distance.truncate(threshold);
            distances.insert(position, distance);
            distances.truncate(threshold);
        }
    }
    sorted_by_distance
}

fn combine(
    input: &[Vec3],
    groups: &mut Vec<HashSet<usize>>,
    sorted_by_distance: &[(usize, usize)],
) -> Result<i128, ()> {
    for (a, b) in sorted_by_distance {
        let a_in = groups.iter().position(|set| set.contains(a));
        let b_in = groups.iter().position(|set| set.contains(b));
        match (a_in, b_in) {
            (None, None) => groups.push([*a, *b].into()),
            (None, Some(b_in)) => {
                groups[b_in].insert(*a);
            }
            (Some(a_in), None) => {
                groups[a_in].insert(*b);
            }
            (Some(a_in), Some(b_in)) if a_in == b_in => {}
            (Some(a_in), Some(b_in)) => {
                let (index, set) = if a_in < b_in {
                    let b_set = groups.remove(b_in);
                    (a_in, b_set)
                } else {
                    let a_set = groups.remove(a_in);
                    (b_in, a_set)
                };
                for x in set {
                    groups[index].insert(x);
                }
            }
        }
        if groups.len() == 1 {
            let range = (0..input.len()).collect::<HashSet<_>>();
            let diff = range.difference(&groups[0]);
            if DEBUG_SOLUTION2 {
                println!("Diff: {diff:?}");
            }
            if diff.count() == 0 {
                return Ok((input[*a].x as i128) * (input[*b].x as i128));
            }
        }
    }
    Err(())
}

fn solution2() -> i128 {
    let input = parse_input();
    const THRESHOLD: usize = 10000;
    let mut sorted_by_distance = prepare_input(&input, THRESHOLD);

    let mut groups = Vec::<HashSet<usize>>::new();

    if DEBUG_SOLUTION2 {
        println!("Iteration 0");
    }
    let mut res = combine(&input, &mut groups, &sorted_by_distance);
    let mut i = 1;
    while res.is_err() {
        if DEBUG_SOLUTION2 {
            println!("Iteration {i}");
        }
        sorted_by_distance = prepare_input_filtered(&input, THRESHOLD, |(a, b)| {
            groups.iter().all(|s| !s.contains(&a) || !s.contains(&b))
        });
        if DEBUG_SOLUTION2 {
            print!("[");
            for s in &groups {
                print!("{},", s.len());
            }
            println!("]");
        }
        res = combine(&input, &mut groups, &sorted_by_distance);
        if DEBUG_SOLUTION2 {
            println!("Groups count: {}", groups.len());
        }
        i += 1;
    }
    res.unwrap()
}

fn parse_input() -> Vec<Vec3> {
    println!("Parsing file: {INPUT_FILE_PATH}");
    let file_path = Path::new(INPUT_FILE_PATH);
    let file = File::open(file_path).unwrap();
    BufReader::new(file)
        .lines()
        .map_while(Result::ok)
        .filter(|l| !l.is_empty())
        .map(|line| {
            let mut iter = line.split(",").map(|x| x.parse::<i64>().unwrap());
            Vec3 {
                x: iter.next().unwrap(),
                y: iter.next().unwrap(),
                z: iter.next().unwrap(),
            }
        })
        .collect::<Vec<_>>()
}
