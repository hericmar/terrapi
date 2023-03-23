use std::collections::hash_map::RandomState;
use std::collections::HashMap;
use std::hash::BuildHasher;
use std::time::SystemTime;
use rand::{Rng, thread_rng};
use rand::distributions::{Alphanumeric, DistString};

const TOKEN_LEN: usize = 32;

#[derive(Clone, Debug)]
pub struct Cache {
    /// hash value, expires in timestamp
    pub tokens: HashMap<String, u64>
}

impl Cache {
    pub fn new() -> Cache {
        Cache{
            tokens: HashMap::new()
        }
    }

    pub fn create_token(&mut self, duration_seconds: u64) -> (String, u64) {
        let token = Alphanumeric.sample_string(&mut rand::thread_rng(), TOKEN_LEN);

        let expires = get_now() + duration_seconds;

        self.tokens.insert(token.clone(), expires);

        (token, expires)
    }

    pub fn validate_token(&mut self, token: &String, duration_seconds: u64) -> bool {
        if !self.tokens.contains_key(token) {
            return false;
        }

        let valid_until = self.tokens.get(token).unwrap();

        if *valid_until < get_now() {
            // token is expired
            self.revoke_token(token);
            return false;
        }

        return true;
    }

    pub fn revoke_token(&mut self, token: &String) -> bool {
        return match self.tokens.remove(token) {
            None => false,
            Some(_) => true
        }
    }
}

fn get_now() -> u64 {
    return match SystemTime::now().duration_since(SystemTime::UNIX_EPOCH) {
        Ok(n) => n.as_secs(),
        Err(_) => panic!("SystemTime before UNIX EPOCH!"),
    };
}
