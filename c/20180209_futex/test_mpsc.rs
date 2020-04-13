use std::{thread, time};
use std::sync::mpsc::sync_channel;

fn main() {
// Create a simple streaming channel
let (tx, rx) = sync_channel(0);

thread::spawn(move|| {
    let now = time::Instant::now();
    println!("sender: {:?}", now);
    tx.send(10).unwrap();
});

let ten_millis = time::Duration::from_millis(100);
thread::sleep(ten_millis);

let r = rx.recv().unwrap();
let now = time::Instant::now();
println!("recver: {:?}, {:?}", now, r);

}
