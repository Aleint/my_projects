BEGIN TRANSACTION;

CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    email TEXT,
    name TEXT,
    salt TEXT,
    password TEXT,
    administrator BOOLEAN NOT NULL
);

CREATE TABLE IF NOT EXISTS pages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT,
    authorId INTEGER,
    authorName TEXT,
    creationDate DATE DEFAULT CURRENT_TIMESTAMP,
    publishDate DATE
);

CREATE TABLE IF NOT EXISTS content_blocks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pageId INTEGER,
    type TEXT,
    orderIndex INTEGER,
    content TEXT,
    imagePath TEXT
);

CREATE TABLE IF NOT EXISTS images (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    imagePath TEXT
);

CREATE TABLE IF NOT EXISTS title (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    titleName TEXT
);


INSERT INTO users VALUES (1, 'enrico@test.com', 'Enrico', '123348dusd437840', 'bddfdc9b092918a7f65297b4ba534dfe306ed4d5d72708349ddadb99b1c526fb', 1); -- password='pwd'
INSERT INTO users VALUES (2, 'luigi@test.com', 'Luigi', '7732qweydg3sd637', '498a8d846eb4efebffc56fc0de16d18905714cf12edf548b8ed7a4afca0f7c1c', 0);
INSERT INTO users VALUES (3, 'alice@test.com', 'Alice', 'wgb32sge2sh7hse7', '09a79c91c41073e7372774fcb114b492b2b42f5e948c61d775ad4f628df0e160', 0);
INSERT INTO users VALUES (4, 'harry@test.com', 'Harry', 'safd6523tdwt82et', '330f9bd2d0472e3ca8f11d147d01ea210954425a17573d0f6b8240ed503959f8', 0);
INSERT INTO users VALUES (5, 'carol@test.com', 'Carol', 'ad37JHUW38wj2833', 'bbbcbac88d988bce98cc13e4c9308306d621d9e278ca62aaee2d156f898a41dd', 0);

INSERT INTO pages VALUES (1, 'Titolo della Pagina1', 1, 'Enrico', '2023-06-01', '2023-06-01');
INSERT INTO pages VALUES (2, 'Titolo della Pagina2', 2, 'Luigi', '2023-06-02', '2023-06-02');

INSERT INTO content_blocks VALUES (1, 1, 'header', 1, 'Header di esempio1', NULL);
INSERT INTO content_blocks VALUES (2, 1, 'paragraph', 2, 'Questo è un paragrafo di esempio1.', NULL);
INSERT INTO content_blocks VALUES (3, 1, 'image', 3, NULL, '/img1.jpg');

INSERT INTO content_blocks VALUES (4, 2, 'header', 1, 'Header di esempio2', NULL);
INSERT INTO content_blocks VALUES (5, 2, 'paragraph', 2, 'Questo è un paragrafo di esempio2.', NULL);
INSERT INTO content_blocks VALUES (6, 2, 'image', 3, NULL, '/img2.png');

INSERT INTO images VALUES (1, '/img1.jpg');
INSERT INTO images VALUES (2, '/img2.png');
INSERT INTO images VALUES (3, '/img3.jpg');
INSERT INTO images VALUES (4, '/img4.jpg');

INSERT INTO title VALUES (1, 'CMSmall');

COMMIT;
