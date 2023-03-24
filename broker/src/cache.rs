use std::collections::HashMap;
use std::time::SystemTime;
use rand::distributions::{Alphanumeric, DistString};
use crate::error::{Error, ErrorType};

const TOKEN_LEN: usize = 32;

#[derive(Debug)]
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

    pub fn validate_token(&mut self, token: &String) -> Result<(), Error> {
        if !self.tokens.contains_key(token) {
            return Err(Error::new("invalid token", ErrorType::Unauthorized))
        }

        let valid_until = self.tokens.get(token).unwrap();

        if *valid_until < get_now() {
            // token is expired
            self.revoke_token(token);
            return Err(Error::new("expired token", ErrorType::Unauthorized))
        }

        Ok(())
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
