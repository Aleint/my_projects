'use strict';
/* Data Access Object (DAO) module for accessing users */

const sqlite = require('sqlite3');
const crypto = require('crypto');

// open the database
const db = new sqlite.Database('cms_db.sqlite', (err) => {
    if (err) throw err;
});

exports.getUserById = (id) => {
    return new Promise((resolve, reject) => {
        const sql = 'SELECT * FROM users WHERE id = ?';
        db.get(sql, [id], (err, row) => {
            if (err)
                reject(err);
            else if (row === undefined)
                resolve({error: 'User not found.'});
            else {
                // by default, the local strategy looks for "username": not to create confusion in server.js, we can create an object with that property
                const user = {id: row.id, username: row.email, name: row.name, administrator: row.administrator}
                resolve(user);
            }
        });
    });
};

exports.getUser = (email, password) => {
    return new Promise((resolve, reject) => {
        const sql = 'SELECT * FROM users WHERE email = ?';
        db.get(sql, [email], (err, row) => {
            if (err) {
                reject(err);
            } else if (row === undefined) {
                resolve(false);
            } else {
                const user = {id: row.id, username: row.email, name: row.name, administrator: row.administrator};

                const salt = row.salt;
                crypto.scrypt(password, salt, 32, (err, hashedPassword) => {
                    if (err) reject(err);

                    const passwordHex = Buffer.from(row.password, 'hex');

                    if (!crypto.timingSafeEqual(passwordHex, hashedPassword))
                        resolve(false);
                    else resolve(user);
                });
            }
        });
    });
}


// get all users
exports.allUsers = () => {
    return new Promise((resolve, reject) => {
        const sql = 'SELECT * FROM users';
        db.all(sql, [], (err, rows) => {
            if (err) {
                reject(err);
                return;
            }
            let users = [];
            rows.forEach((row) => {
                const user = {
                    id: row.id,
                    email: row.email,
                    name: row.name,
                };
                users.push(user);
            });
            resolve(users);
        });
    });
};
  
