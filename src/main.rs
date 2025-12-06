use aes_gcm::{Aes256Gcm, KeyInit, Nonce};
use aes_gcm::aead::{Aead, OsRng, AeadCore, AeadInPlace};
use argon2::{Argon2, PasswordHasher, password_hash::{SaltString, PasswordHash, PasswordVerifier}};
use rand::RngCore;
use walkdir::WalkDir;
use std::fs::{self, File};
use std::io::{Read, Write};
use anyhow::{Result, Context};
use std::path::Path;

fn derive_key(password: &str, salt: &[u8]) -> [u8; 32] {
    let argon2 = Argon2::default();
    let mut key = [0u8; 32];
    argon2.hash_password_into(password.as_bytes(), salt, &mut key)
        .expect("Key derivation failed");
    key
}

fn encrypt_file(path: &Path, key: &[u8; 32]) -> Result<()> {
    let mut file = File::open(path).context("Opening file for encryption failed")?;
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer)?;

    let cipher = Aes256Gcm::new(key.into());

    let mut nonce_bytes = [0u8; 12];
    OsRng.fill_bytes(&mut nonce_bytes);
    let nonce = Nonce::from_slice(&nonce_bytes);

    let ciphertext = cipher.encrypt(nonce, buffer.as_ref())
        .context("Encryption failed")?;

    let mut out = File::create(path.with_extension("enc"))?;
    out.write_all(&nonce_bytes)?;
    out.write_all(&ciphertext)?;

    fs::remove_file(path)?;
    Ok(())
}

fn decrypt_file(path: &Path, key: &[u8; 32]) -> Result<()> {
    let mut file = File::open(path).context("Opening file for decryption failed")?;
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer)?;

    let nonce_bytes = &buffer[..12];
    let ciphertext = &buffer[12..];

    let cipher = Aes256Gcm::new(key.into());
    let nonce = Nonce::from_slice(nonce_bytes);

    let plaintext = cipher.decrypt(nonce, ciphertext)
        .context("Decryption failed")?;

    let orig_path = path.with_extension(""); // removes ".enc"
    let mut out = File::create(orig_path)?;
    out.write_all(&plaintext)?;

    fs::remove_file(path)?;
    Ok(())
}

fn encrypt_folder(folder: &Path, key: &[u8; 32]) -> Result<()> {
    for entry in WalkDir::new(folder) {
        let entry = entry?;
        if entry.file_type().is_file() && !entry.path().extension().map_or(false, |ext| ext == "enc") {
            encrypt_file(entry.path(), key)?;
        }
    }
    Ok(())
}

fn decrypt_folder(folder: &Path, key: &[u8; 32]) -> Result<()> {
    for entry in WalkDir::new(folder) {
        let entry = entry?;
        if entry.file_type().is_file() && entry.path().extension().map_or(false, |ext| ext == "enc") {
            decrypt_file(entry.path(), key)?;
        }
    }
    Ok(())
}

fn main() -> Result<()> {
    let password = "supersecretpassword"; // Choose or input securely
    let salt = b"unique_salt_16b"; // 16 bytes salt for key derivation
    let key = derive_key(password, salt);

    let target_folder = std::path::Path::new("data");

    encrypt_folder(target_folder, &key)?;
    decrypt_folder(target_folder, &key)?;

    Ok(())
}