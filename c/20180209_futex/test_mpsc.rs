use std::{thread, time};
use std::sync::mpsc::channel;

fn main() {
// Create a simple streaming channel
let (tx, rx) = channel();

thread::spawn(move|| {
let ten_millis = time::Duration::from_millis(1000);
thread::sleep(ten_millis);


    let now = time::Instant::now();
    println!("sender: {:?}", now);
    tx.send(10).unwrap();
});


let r = rx.recv().unwrap();
let now = time::Instant::now();
println!("recver: {:?}, {:?}", now, r);

}
