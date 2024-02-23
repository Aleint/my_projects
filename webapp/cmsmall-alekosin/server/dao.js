'use strict';

const sqlite = require('sqlite3');
const dayjs = require('dayjs');

// open the database
const db = new sqlite.Database('cms_db.sqlite', (err) => {
    if (err) throw err
});


// get all pages
exports.listPages = () => {
    return new Promise((resolve, reject) => {
        const sql = 'SELECT p.id, p.title, p.authorId,p.authorName, p.creationDate, p.publishDate, cb.id AS blockId, cb.type, cb.orderIndex, cb.content, cb.imagePath FROM pages p JOIN content_blocks cb ON p.id = cb.pageId';
        db.all(sql, [], (err, rows) => {
            if (err) {
                reject(err);
                return;
            }
            const pages = [];
            const pageMap = new Map();//By using a map,
            // duplicate page objects with the same ID are prevented,
            // ensuring that each page appears only once in the pages array

            rows.forEach((row) => {
                const pageId = row.id;
                if (!pageMap.has(pageId)) {
                    const page = {
                        id: row.id,
                        title: row.title,
                        authorId: row.authorId,
                        authorName: row.authorName,
                        creationDate: dayjs(row.creationDate),
                        publishDate: row.publishDate ? dayjs(row.publishDate) : '',
                        blocks: [],
                    };
                    pageMap.set(pageId, page);
                    pages.push(page);
                }

                const block = {
                    id: row.blockId,
                    type: row.type,
                    orderIndex: row.orderIndex,
                    content: row.content,
                    imagePath: row.imagePath,
                };

                const page = pageMap.get(pageId);
                page.blocks.push(block);
            });

            resolve(pages);
        });
    });
};


// get the page identified by {id}
exports.getPage = (id) => {
    return new Promise((resolve, reject) => {
        const sql =
            'SELECT p.id, p.title, p.authorId,p.authorName, p.creationDate, p.publishDate, cb.id AS blockId, cb.type, cb.orderIndex, cb.content, cb.imagePath FROM pages p JOIN content_blocks cb ON p.id = cb.pageId WHERE p.id=? ORDER BY cb.orderIndex';
        db.all(sql, [id], (err, rows) => {
            if (err) {
                reject(err);
                return;
            }
            if (rows.length === 0) {
                resolve({error: 'Page not found.'});
            } else {
                const page = {
                    id: rows[0].id,
                    title: rows[0].title,
                    authorId: rows[0].authorId,
                    authorName: rows[0].authorName,
                    creationDate: rows[0].creationDate,
                    publishDate: rows[0].publishDate,
                    contentBlocks: [],
                };

                rows.forEach((row) => {
                    const contentBlock = {
                        id: row.blockId,
                        type: row.type,
                        orderIndex: row.orderIndex,
                        content: row.content,
                        imagePath: row.imagePath,
                    };

                    page.contentBlocks.push(contentBlock);
                });

                resolve(page);
            }
        });
    });
};

// get all contents of the page {pageId}
exports.getContents = (pageId) => {
    return new Promise((resolve, reject) => {
        const sql = 'SELECT * FROM content_blocks WHERE pageId = ? ORDER BY orderIndex';
        db.all(sql, [pageId], (err, rows) => {
            if (err) {
                reject(err);
                return;
            }
            if (rows == undefined) {
                resolve({error: 'Contents not found.'});
            } else {
                const contents = rows.map((e) => ({
                    id: e.id,
                    pageId: e.pageId,
                    type: e.type,
                    orderIndex: e.orderIndex,
                    content: e.content,
                    imagePath: e.imagePath
                }));
                resolve(contents);
            }
        });
    });
};

// get all images
exports.listImages = () => {
    return new Promise((resolve, reject) => {
        const sql = 'SELECT * FROM images';
        db.all(sql, (err, rows) => {
            if (err) {
                reject(err);
                return;
            }
            if (rows.length === 0) {
                resolve({error: 'Images not found.'});
            } else {
                resolve(rows);
            }

        });
    });
};

// get title of website
exports.getTitle = () => {
    return new Promise((resolve, reject) => {
        const sql = 'SELECT * FROM title';
        db.all(sql, (err, rows) => {
            if (err) {
                reject(err);
                return;
            }
            if (rows.length === 0) {
                resolve({error: 'title not found.'});
            } else {
                resolve(rows);
            }

        });
    });
};


// add a new page
exports.createPage = (page) => {
    return new Promise((resolve, reject) => {
        const sql = 'INSERT INTO pages(title, authorId,authorName, creationDate, publishDate) VALUES(?, ?, ?, ?,?)';
        const {title, authorId, authorName, creationDate, publishDate, blocks} = page;
        db.run(sql, [title, authorId, authorName, creationDate, publishDate], function (err) {
            if (err) {
                reject(err);
                return;
            }

            const pageId = this.lastID;
            if (blocks && blocks.length > 0) {
                const blockSql = 'INSERT INTO content_blocks(pageId, type, orderIndex, content, imagePath) VALUES(?, ?, ?, ?, ?)';
                const blockValues = blocks.map((block) => [
                    pageId,
                    block.type,
                    block.orderIndex,
                    block.content,
                    block.imagePath,
                ]);

                db.serialize(() => {  //db.serialize() call to ensure that the database operations are executed sequentially
                    blockValues.forEach((values) => {
                        db.run(blockSql, values, (err) => {
                            if (err) {
                                reject(err);
                                return;
                            }
                        });
                    });
                });
            } else {
                reject('No blocks provided')
            }

            resolve(pageId);
        });
    });
};


// update an existing page
exports.updatePage = (page, user) => {
    return new Promise((resolve, reject) => {
        const {id, title, authorId, authorName, creationDate, publishDate, blocks} = page;

        // Update the "pages" table
        if (user.administrator) {
            const updatePageSql = 'UPDATE pages SET title = ?, authorId = ?,authorName=?, creationDate = ?, publishDate = ? WHERE id = ? ';
            db.run(updatePageSql, [title, authorId, authorName, creationDate, publishDate, id], function (err) {
                if (err) {
                    reject(err);
                    return;
                }
            });
        } else {
            const updatePageSql = 'UPDATE pages SET title = ?, authorId = ?,authorName=?, creationDate = ?, publishDate = ? WHERE id = ? AND authorId=?';
            db.run(updatePageSql, [title, authorId, authorName, creationDate, publishDate, id, user.id], function (err) {
                if (err) {
                    reject(err);
                    return;
                }
            });

        }

        // Update the "content_blocks" table if blocks are provided
        if (blocks && blocks.length > 0) {
            // Delete existing blocks for the page
            const sql = 'DELETE FROM content_blocks WHERE pageId = ?';
            db.run(sql, [id], function (err) {
                if (err) {
                    reject(err);
                    return;
                }

                // Insert the updated blocks
                const blockSql = 'INSERT INTO content_blocks(pageId, type, orderIndex, content, imagePath) VALUES (?, ?, ?, ?, ?)';
                const blockValues = blocks.map((block) => [id, block.type, block.orderIndex, block.content, block.imagePath]);
                blockValues.forEach((values) => {
                    db.run(blockSql, values, (err) => {
                        if (err) {
                            reject(err);
                            return;
                        }
                    });
                });

                resolve();
            });
        } else {
            reject('No blocks provided')

        }

    });
};


// delete an existing page
exports.deletePage = (id, userId) => {
    return new Promise((resolve, reject) => {
        let sql;
        const params = [id, userId];


        if (userId === 'admin') {
            sql = 'DELETE FROM pages WHERE id = ?';
            db.run(sql, [id], function (err) {
                if (err) {
                    reject(err);
                    return;
                }
            });
        } else {
            sql = 'DELETE FROM pages WHERE id = ? AND authorId = ?';
            db.run(sql, params, function (err) {
                if (err) {
                    reject(err);
                    return;
                }
            });
        }

        const deleteBlocksSql = 'DELETE FROM content_blocks WHERE pageId = ?';
        db.run(deleteBlocksSql, [id], function (err) {
            if (err) {
                reject(err);
                return;
            }
            resolve({
                pageChanges: this.changes, // number of affected rows in pages table
                blockChanges: this.changes, // number of affected rows in content_blocks table
            });
        });


    });
};

exports.updateTitle = (title, user) => {
    return new Promise((resolve, reject) => {
        // Update the "title" table
        if (user.administrator) {
            const updateTitleSql = 'UPDATE title SET titleName = ? WHERE id = ?';
            db.run(updateTitleSql, [title.titleName, title.id], function (err) {
                if (err) {
                    reject(err);
                    return;
                }
                resolve();
            });
        } else {
            reject('Unauthorized');
        }
    });
};

