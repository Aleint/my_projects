import 'bootstrap/dist/css/bootstrap.min.css';
import {Button, Container} from 'react-bootstrap';
import {useEffect, useState} from 'react';
import {BrowserRouter, Link, Navigate, Route, Routes, useNavigate, useParams} from 'react-router-dom';
import NavHeader from './components/NavbarComponents';
import PageRoute from './components/PagesRoute';
import FormRoute from './components/FormRoute.jsx';
import {LoginForm} from './components/AuthComponents';
import API from './API';
import {Alert, Dialog, DialogActions, DialogContent, DialogContentText, DialogTitle} from "@mui/material";
import dayjs from 'dayjs';

function DefaultRoute() {
    return (
        <Container className='App'>
            <h1>No data here...</h1>
            <h2>This is not the route you are looking for!</h2>
            <Link to='/'>Please go back to the main page</Link>
        </Container>
    );
}

function App() {
    const [pages, setPages] = useState([]);
    const [users, setUsers] = useState([]);
    const [pubPages, setPubPages] = useState([]);
    const [images, setImages] = useState();
    const [initialLoading, setInitialLoading] = useState(true);
    const [dirty, setDirty] = useState(true);
    const [errorMsg, setErrorMsg] = useState('');
    const [user, setUser] = useState(undefined);
    const [title, setTitle] = useState('');
    const [loggedIn, setLoggedIn] = useState(false);
    const [showSuccessMessage, setShowSuccessMessage] = useState(false);

    function handleError(err) {
        let errMsg = 'Unknown error';
        if (err.errors) {
            if (err.errors[0])
                if (err.errors[0].msg)
                    errMsg = err.errors[0].msg;
        } else if (err.error) {
            errMsg = err.error;
        }
        setErrorMsg(errMsg);
        setDirty(true)
    }

    useEffect(() => {
        const checkAuth = async () => {
            try {
                // here you have the user info, if already logged in
                const user = await API.getUserInfo();
                setLoggedIn(true);
                setUser(user);
            } catch (err) {
                // NO need to do anything: the user is simply not yet authenticated
                //handleError(err);
            }
        };
        checkAuth();
    }, []);

    useEffect(() => {
        if (dirty) {
            API.getTitle()
                .then((title) => {
                    setTitle(title[0]);
                    setInitialLoading(false);
                })
                .catch((err) => handleError(err));
            API.getAllPages()
                .then((pages) => {
                    setPages(pages);
                    setDirty(false);
                    setInitialLoading(false);
                })
                .catch((err) => handleError(err));
        }
    }, [dirty, pages]);

    useEffect(() => {
        API.getAllImages()
            .then((images) => {
                setImages(images);
                setInitialLoading(false);
            })
            .catch((err) => handleError(err));

    }, []);

    useEffect(() => {
        if (user && user.administrator)
            API.getUsers()
                .then((users) => {
                    setUsers(users);
                    setInitialLoading(false);
                });
    }, [user])

    useEffect(() => {
        setPubPages(
            pages.filter(
                (e) =>
                    e.publishDate !== null &&
                    dayjs(e.publishDate).format('YYYY-MM-DD') <= dayjs().format('YYYY-MM-DD')
            )
        );
    }, [pages]);

    const doLogOut = async () => {
        await API.logOut();
        setLoggedIn(false);
        setUser(undefined);
    };

    const loginSuccessful = (user) => {
        setUser(user);
        setLoggedIn(true);
        setDirty(true);
    };

    const addPage = (e) => {
        e.pageId = e.id;

        setPages((oldList) => {
            const newTempId = Math.max(...oldList.map((e) => e.id)) + 1;
            e.id = newTempId;
            e.status = 'added';
            return [...oldList, e];
        });

        API.addPage(e)
            .then(() => setDirty(true))
            .catch((err) => handleError(err));
    };

    const editPage = (newEl) => {
        setPages((oldList) =>
            oldList.map((e) => {
                if (e.id === newEl.id) {
                    newEl.status = 'updated';
                    return newEl;
                } else {
                    return e;
                }
            })
        );

        API.editPage(newEl)
            .then(() => {
                setDirty(true);
            })
            .catch((err) => handleError(err));
    };

    const editTitle = (newEl) => {
        setTitle((oldTitle) => {
            if (oldTitle.id === newEl.id) {
                newEl.status = 'updated';
                return newEl;
            } else {
                return oldTitle;
            }
        });

        API.editTitle(newEl)
            .then(() => {
                setDirty(true);
                setShowSuccessMessage(true);
                setTimeout(() => setShowSuccessMessage(false), 3000);
            })
            .catch((err) => handleError(err));
    };

    function DeleteRoute(props) {
        const [open, setOpen] = useState(true);
        const {pageId, title} = useParams();
        const navigate = useNavigate();

        const handleClose = () => {
            setOpen(false);
            navigate('/');
        };

        const handleDelete = () => {
            props.setPages((oldList) =>
                oldList.map((e) =>
                    e.id !== pageId ? e : Object.assign({}, e, {status: 'deleted'})
                )
            );

            API.deletePage(pageId)
                .then(() => {
                    setOpen(false);
                    setDirty(true);
                    navigate('/');
                })
                .catch((error) => {
                    console.error('Error deleting page:' + title, error);
                });
        };

        return (
            <div>
                <Dialog open={open} onClose={handleClose}>
                    <DialogTitle>Delete</DialogTitle>
                    <DialogContent>
                        <DialogContentText>
                            Sure to delete: {title} ?
                        </DialogContentText>
                    </DialogContent>
                    <DialogActions>
                        <Button onClick={handleClose}>Cancel</Button>
                        <Button onClick={handleDelete} variant='danger'>
                            Delete
                        </Button>
                    </DialogActions>
                </Dialog>
            </div>
        );
    }

    return (
        <BrowserRouter>
            <NavHeader
                editTitle={editTitle}
                title={title}
                setTitle={setTitle}
                user={user}
                logout={doLogOut}
            />
            {showSuccessMessage && (
                <Alert variant="success" className='bg-success'>
                    Title updated
                </Alert>
            )}
            <Routes>
                <Route
                    path='/'
                    element={
                        <PageRoute
                            initialLoading={initialLoading}
                            errorMsg={errorMsg}
                            resetErrorMsg={() => setErrorMsg('')}
                            pages={pages}
                            pubPages={pubPages}
                            loggedIn={loggedIn}
                            user={user}
                            users={users}
                        />
                    }
                />
                <Route
                    path='/add'
                    element={<FormRoute user={user}
                                        logout={doLogOut}
                                        pageList={pages}
                                        addPage={addPage}
                                        editPage={editPage}
                                        images={images}
                                        users={users}/>}
                />
                <Route
                    path='/edit/:pageId'
                    element={
                        <FormRoute
                            user={user}
                            logout={doLogOut}
                            pageList={pages}
                            addPage={addPage}
                            editPage={editPage}
                            images={images}
                            users={users}
                        />
                    }
                />
                <Route
                    path='delete/:pageId/:title'
                    element={<DeleteRoute setPages={setPages} setPubPages={setPubPages}/>}
                />
                <Route
                    path='/login'
                    element={
                        loggedIn ? (
                            <Navigate replace to='/'/>
                        ) : (
                            <LoginForm loginSuccessful={loginSuccessful}/>
                        )
                    }
                />
                <Route path='/*' element={<DefaultRoute/>}/>
            </Routes>
        </BrowserRouter>
    );
}

export default App;
