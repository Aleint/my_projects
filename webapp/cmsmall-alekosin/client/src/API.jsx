/**
 * All the API calls
 */

import dayjs from "dayjs";

const URL = 'http://localhost:3001/api';


async function getAllPages() {
    const response = await fetch(URL + '/pages');
    const pages = await response.json();
    if (response.ok) {
        const sortedPages = pages
            .map((e) => ({
                id: e.id,
                title: e.title,
                authorId: e.authorId,
                authorName: e.authorName,
                creationDate: dayjs(e.creationDate),
                publishDate: e.publishDate ? dayjs(e.publishDate) : '',
                blocks: e.blocks,
            }))
            .filter((page) => page.publishDate !== '')
            .sort((a, b) => a.publishDate.diff(b.publishDate));

        const pagesWithoutPublishDate = pages
            .filter((page) => page.publishDate === '')
            .map((e) => ({
                id: e.id,
                title: e.title,
                authorId: e.authorId,
                authorName: e.authorName,
                creationDate: dayjs(e.creationDate),
                publishDate: '',
                blocks: e.blocks,
            }));

        const combinedPages = [...sortedPages, ...pagesWithoutPublishDate];

        return combinedPages;
    } else {
        throw pages;
    }
}

async function getAllImages() {
    // call  /api/images
    const response = await fetch(URL + '/images');
    const images = await response.json();
    if (response.ok) {
        return images
            .map((e) => ({
                    id: e.id,
                    imagePath: e.imagePath
                })
            )
    } else {
        throw images;
    }
}

async function getTitle() {
    // call  /api/title
    const response = await fetch(URL + '/title');
    const title = await response.json();
    if (response.ok) {
        return title
            .map((e) => ({
                    id: e.id,
                    titleName: e.titleName
                })
            )
    } else {
        throw title;
    }
}

function addPage(page) {
    // call  POST /api/pages
    return new Promise((resolve, reject) => {
        fetch(URL + `/pages`, {
            method: 'POST',
            credentials: 'include',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(Object.assign({}, page)),
        }).then((response) => {
            if (response.ok) {
                response.json()
                    .then((id) => resolve(id))
                    .catch(() => {
                        reject({error: "Cannot parse server response."})
                    });
            } else {
                // analyze the cause of error
                response.json()
                    .then((message) => {
                        reject(message);
                    }) // error message in the response body
                    .catch(() => {
                        reject({error: "Cannot parse server response."})
                    });
            }
        }).catch(() => {
            reject({error: "Cannot communicate with the server."})
        }); // connection errors
    });
}


function deletePage(id) {
    // call  DELETE /api/pages/<id>
    return new Promise((resolve, reject) => {
        fetch(URL + `/pages/${id}`, {
            method: 'DELETE',
            credentials: 'include',
        }).then((response) => {
            if (response.ok) {
                resolve(null);
            } else {
                // analyze the cause of error
                response.json()
                    .then((message) => {
                        reject(message);
                    }) // error message in the response body
                    .catch(() => {
                        reject({error: "Cannot parse server response."})
                    });
            }
        }).catch(() => {
            reject({error: "Cannot communicate with the server."})
        }); // connection errors
    });
}

function editPage(page) {
    // call  PUT /api/pages/<id>
    return new Promise((resolve, reject) => {
        fetch(URL + `/pages/${page.id}`, {
            method: 'PUT',
            credentials: 'include',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(Object.assign({}, page)),
        }).then((response) => {
            if (response.ok) {
                resolve(null);
            } else {
                // analyze the cause of error
                response.json()
                    .then((message) => {
                        reject(message);
                    }) // error message in the response body
                    .catch(() => {
                        reject({error: "Cannot parse server response."})
                    });
            }
        }).catch(() => {
            reject({error: "Cannot communicate with the server."})
        });
    });
}

function editTitle(title) {
    // call  PUT /api/pages/<id>
    return new Promise((resolve, reject) => {
        fetch(URL + '/title', {
            method: 'PUT',
            credentials: 'include',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(Object.assign({}, title)),
        }).then((response) => {
            if (response.ok) {
                resolve(null);
            } else {
                // analyze the cause of error
                response.json()
                    .then((message) => {
                        reject(message);
                    }) // error message in the response body
                    .catch(() => {
                        reject({error: "Cannot parse server response."})
                    }); // something else
            }
        }).catch(() => {
            reject({error: "Cannot communicate with the server."})
        }); // connection errors
    });
}


async function logIn(credentials) {
    let response = await fetch(URL + '/sessions', {
        method: 'POST',
        credentials: 'include',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(credentials),
    });
    if (response.ok) {
        const user = await response.json();
        return user;
    } else {
        const errDetail = await response.json();
        throw errDetail.message;
    }
}

async function logOut() {
    await fetch(URL + '/sessions/current', {
        method: 'DELETE',
        credentials: 'include'
    });
}

async function getUserInfo() {
    const response = await fetch(URL + '/sessions/current', {
        credentials: 'include'
    });
    const userInfo = await response.json();
    if (response.ok) {
        return userInfo;
    } else {
        throw userInfo;
    }
}

async function getUsers() {
    // call  /api/users
    const response = await fetch(URL + '/users',{credentials: 'include'});
    const users = await response.json();
    if (response.ok) {

        return users.map((e) => ({
            id: e.id,
            email: e.email,
            name: e.name,
        }));
    } else {
        throw users;
    }
}


const API = {
    getAllPages, getAllImages, getUsers, logOut, getTitle, editTitle, addPage, editPage, deletePage, getUserInfo, logIn
};
export default API;