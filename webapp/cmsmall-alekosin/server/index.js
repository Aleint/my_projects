'use strict';
const express = require('express');
const morgan = require('morgan'); // logging middleware
const {check, validationResult} = require('express-validator'); // validation middleware
const dao = require('./dao'); // module for accessing the DB
const passport = require('passport'); // auth middleware
const LocalStrategy = require('passport-local').Strategy; // username and password for login
const session = require('express-session'); // enable sessions
const userDao = require('./user-dao'); // module for accessing the user info in the DB
const cors = require('cors');


/*** Set up Passport ***/
// set up the "username and password" login strategy
// by setting a function to verify username and password
passport.use(new LocalStrategy(
    function (username, password, done) {
        userDao.getUser(username, password).then((user) => {
            if (!user)
                return done(null, false, {message: 'Incorrect username and/or password.'});

            return done(null, user);
        })
    }
));

// serialize and de-serialize the user (user object <-> session)
// we serialize the user id and we store it in the session: the session is very small in this way
passport.serializeUser((user, done) => {
    done(null, user.id);
});

// starting from the data in the session, we extract the current (logged-in) user
passport.deserializeUser((id, done) => {
    userDao.getUserById(id)
        .then(user => {
            done(null, user); // this will be available in req.user
        }).catch(err => {
        done(err, null);
    });
});

// init express
const app = express();
const port = 3001;

// set-up the middlewares
app.use(express.static('images'));
app.use(morgan('dev'));
app.use(express.json());
const corsOptions = {
    origin: 'http://localhost:5173',
    credentials: true,
};
app.use(cors(corsOptions));


// custom middleware: check if a given request is coming from an authenticated user
const isLoggedIn = (req, res, next) => {
    if (req.isAuthenticated())
        return next();

    return res.status(401).json({error: 'Not authenticated'});
}
// set up the session
app.use(session({
    // by default, Passport uses a MemoryStore to keep track of the sessions
    secret: 'wge8d239bwd93rks23',   //personalize this random string, should be a secret value
    resave: false,
    saveUninitialized: false
}));

// then, init passport
app.use(passport.initialize());
app.use(passport.session());
/*** APIs ***/


// GET /api/pages
app.get('/api/pages', (req, res) => {
    dao.listPages()
        .then(pages => res.json(pages))
        .catch((err) => {
            console.log(err);
            res.status(500).end()
        });
});

// GET /api/pages/<id>
app.get('/api/pages/:id',isLoggedIn, (req, res) => {
    dao.getContents(req.params.id).then(contents => res.json(contents))
        .catch((err) => {
            console.log(err);
            res.status(500).end()
        });
});

// GET /api/images
app.get('/api/images', (req, res) => {
    dao.listImages()
        .then(images => res.json(images))
        .catch((err) => {
            console.log(err);
            res.status(500).end()
        });
});

// GET /api/title
app.get('/api/title', (req, res) => {
    dao.getTitle()
        .then(title => res.json(title))
        .catch((err) => {
            console.log(err);
            res.status(500).end()
        });
});
// POST /api/pages
app.post('/api/pages', isLoggedIn, [
    check('creationDate').isDate({format: 'YYYY-MM-DD', strictMode: true}),
    check('title').notEmpty()
], async (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
        return res.status(422).json({errors: errors.array()});
    } else {
        const page = {
            title: req.body.title,
            authorId: req.user.administrator ? req.body.authorId : req.user.id,
            authorName: req.user.administrator ? req.body.authorName : req.user.name,
            creationDate: req.body.creationDate,
            publishDate: req.body.publishDate,
            blocks: req.body.blocks
        };

        try {
            const pageId = await dao.createPage(page);
            res.status(201).json(pageId);
        } catch (err) {
            console.log(err);
            res.status(503).json({error: `Database error during the creation of page ${page.title} by ${page.authorName}.`});
        }
    }
});


// PUT /api/pages/<id>
app.put('/api/pages/:id', isLoggedIn, [
    check('creationDate').isDate({format: 'YYYY-MM-DD', strictMode: true}),
    check('title').notEmpty()
], async (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
        return res.status(422).json({errors: errors.array()});
    }

    const pageId = req.params.id;
    const resultPage = await dao.getPage(pageId); // Retrieve the existing page to ensure it exists
    if (resultPage.error) {
        res.status(404).json(resultPage);
    } else {
        const page = {
            id: pageId,
            title: req.body.title,
            authorId: req.user.administrator ? req.body.authorId : req.user.id,
            authorName: req.user.administrator ? req.body.authorName : req.user.name,
            creationDate: req.body.creationDate,
            publishDate: req.body.publishDate,
            blocks: req.body.blocks
        };

        try {
            await dao.updatePage(page, req.user);
            res.status(200).json({message: 'Page updated successfully'});
        } catch (err) {
            console.log(err);
            res.status(503).json({error: `Database error during the update of page ${pageId}.`});
        }
    }
});
// PUT /api/title
app.put('/api/title', isLoggedIn, [
    check('titleName').notEmpty()
], async (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
        return res.status(422).json({errors: errors.array()});
    }
    const title = req.body;
    try {
        await dao.updateTitle(title, req.user); // Call a method to update the page in the database
        res.status(200).json({message: 'website title updated successfully'});
    } catch (err) {
        console.log(err);
        res.status(503).json({error: 'Database error during the update of website title.'});
    }
});


// DELETE /api/pages/<id>
app.delete('/api/pages/:id', isLoggedIn, async (req, res) => {
    try {
        const numRowChanges = await dao.deletePage(req.params.id, req.user.administrator ? 'admin' : req.user.id)
        res.json(numRowChanges);
    } catch (err) {
        console.log(err);
        res.status(503).json({error: `Database error during the deletion of page ${req.params.id}.`});
    }
});


/*** Users APIs ***/

// POST /sessions
// login
app.post('/api/sessions', function (req, res, next) {
    passport.authenticate('local', (err, user, info) => {
        if (err)
            return next(err);
        if (!user) {
            // display wrong login messages
            return res.status(401).json(info);
        }
        // success, perform the login
        req.login(user, (err) => {
            if (err)
                return next(err);

            // req.user contains the authenticated user, we send all the user info back
            // this is coming from userDao.getUser()
            return res.json(req.user);
        });
    })(req, res, next);
});

// DELETE /sessions/current
// logout
app.delete('/api/sessions/current', (req, res) => {
    req.logout(() => {
        res.end();
    });
});

// GET /sessions/current
// check whether the user is logged in or not
app.get('/api/sessions/current', (req, res) => {
    if (req.isAuthenticated()) {
        res.status(200).json(req.user);
    } else
        res.status(401).json({error: 'Unauthenticated user!'});

});

// GET /api/users
app.get('/api/users', isLoggedIn, (req, res) => {
    if(req.user.administrator){
        userDao.allUsers()
            .then(users => res.json(users))
            .catch((err) => {
                console.log(err);
                res.status(500).end()
            });
    }
    else {
        return res.status(401).end();
    }

});


// Activate the server
app.listen(port, () => {
    console.log(`react-server listening at http://localhost:${port}`);
});
