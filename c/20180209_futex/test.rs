use std::{thread, time};

fn main() {

let ten_millis = time::Duration::from_millis(10);
let now = time::Instant::now();

thread::sleep(ten_millis);

assert!(now.elapsed() >= ten_millis);
}
