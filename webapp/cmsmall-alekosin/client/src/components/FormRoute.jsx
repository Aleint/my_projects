import {Alert, Button, Card, Container, Form} from 'react-bootstrap';
import * as React from 'react';
import {useState} from 'react';
import {Link, useNavigate, useParams} from 'react-router-dom';
import dayjs from 'dayjs';
import {ImageList, ImageListItem} from '@mui/material';

const BLOCK_TYPES = ['header', 'paragraph', 'image'];
const serverBaseUrl = 'http://localhost:3001';


function FormRoute(props) {
    return (
        <Container fluid>
            <PageForm
                pageList={props.pageList}
                addPage={props.addPage}
                editPage={props.editPage}
                user={props.user}
                images={props.images}
                users={props.users}
            />
        </Container>
    );
}

function PageForm(props) {
    const navigate = useNavigate();

    const {pageId} = useParams();
    const {users} = props;

    const objToEdit = pageId && props.pageList.find((e) => e.id === parseInt(pageId));

    const [title, setTitle] = useState(objToEdit ? objToEdit.title : '');
    const [authorId, setAuthorId] = useState(objToEdit ? objToEdit.authorId : props.user.id);
    const [authorName, setAuthorName] = useState(objToEdit ? objToEdit.authorName : props.user.name);
    const [publishDate, setPublishDate] = useState(objToEdit && objToEdit.publishDate ? dayjs(objToEdit.publishDate).format('YYYY-MM-DD') : '');
    const [creationDate, setCreationDate] = useState(objToEdit ? dayjs(objToEdit.creationDate).format('YYYY-MM-DD') : '');
    const [blockType, setBlockType] = useState(BLOCK_TYPES[0]);
    const [errorMsg, setErrorMsg] = useState('');

    const [blocks, setBlocks] = useState(objToEdit ? objToEdit.blocks : []);


    const handleTitle = (ev) => {
        const v = ev.target.value;
        setTitle(v);
    };

    const handleDate = (ev) => {
        const v = ev.target.value;
        setPublishDate(v);
    };

    const handleBlockContent = (blockId, content) => {
        const updatedBlocks = blocks.map((block) => (block.id === blockId ? {...block, content} : block));
        setBlocks(updatedBlocks);
    };

    const handleBlockOrderChange = (blockId, newOrder, action) => {
        const blockIndex = blocks.findIndex((block) => block.id === blockId);
        const block = blocks[blockIndex];

        // Remove the block from the current position
        const updatedBlocks = blocks.filter((block) => block.id !== blockId);

        // Check if there are any blocks with the same order index as the newOrder
        const insertIndex = updatedBlocks.findIndex((block) => block.orderIndex === newOrder);

        if (insertIndex === -1) {
            // No blocks with the same order index, append the block at the end
            updatedBlocks.push(block);
        } else {
            if (action === 'decrease') {
                // Insert the block before the block with the same order index
                updatedBlocks.splice(insertIndex, 0, block);
            } else {
                // Insert the block after the block with the same order index
                updatedBlocks.splice(insertIndex + 1, 0, block);

            }

        }

        // Update the order index for all blocks
        const updatedBlocksOrdered = updatedBlocks.map((block, index) => ({
            ...block,
            orderIndex: index + 1, // Bounded orderIndex from 1 to blocks.length+1
        }));

        setBlocks(updatedBlocksOrdered);
    };


    const handleBlockTypeChange = (blockId, type, image = props.images[0]) => {
        let updatedBlocks;
        if (type === 'image') {
            updatedBlocks = blocks.map((block) => (block.id === blockId ? {
                ...block,
                imagePath: image.imagePath,
                type
            } : block));
        } else {
            updatedBlocks = blocks.map((block) => (block.id === blockId ? {...block, type, imagePath: ''} : block));
        }
        setBlocks(updatedBlocks);
        setBlockType(type);
    };

    const handleSubmit = (event) => {
        event.preventDefault();
        setCreationDate(creationDate?creationDate:dayjs().format('YYYY-MM-DD'));
        if (title === '') {
            setErrorMsg('Title is required');
        } else if (blocks.length === 0 || !blocks.some((block) => block.type !== 'header')) {
            setErrorMsg('Add at least one header and one other block');
        } else if (!blocks.some((block) => block.type === 'header')) {
            setErrorMsg('Add at least one header');
        } else if (blocks.some((block) => block.content === '' && block.type !== 'image')) {
            setErrorMsg('Empty block content');
        } else if (dayjs(creationDate).isAfter(publishDate)) {
            setErrorMsg(`This is not a travel machine! Choose a publishDate after the creation date: ${dayjs(creationDate).format('YYYY-MM-DD')}`);
        } else {

            const page = {
                title,
                authorId,
                authorName,
                publishDate: publishDate ? dayjs(publishDate).format('YYYY-MM-DD') : '',
                creationDate,
                blocks,
            };

            if (objToEdit) {
                page.id = objToEdit.id;
                props.editPage(page);
            } else {
                page.creationDate = dayjs().format('YYYY-MM-DD');
                props.addPage(page);
            }
            navigate('/');
        }
    };

    const createBlock = (type, content = '', imagePath =type==='image'?props.images[0].imagePath:'') => {
        const newBlock = {
            id: blocks.length === 0 ? 0 : Math.max(...blocks.map((e) => e.id)) + 1,
            content,
            type,
            imagePath,
            orderIndex: blocks.length + 1,
        };
        return newBlock;
    };

    function handleUserChange(ev) {
        const email = ev.target.value;
        let author = users.find(user => user.email === email);
        setAuthorId(author.id)
        setAuthorName(author.name)
    }

    return (
        <>

            <Form onSubmit={handleSubmit}>
                <Form.Group className="mb-3">
                    <Form.Label>Title</Form.Label>
                    <Form.Control type="text" name="title" value={title} onChange={handleTitle}/>
                </Form.Group>

                {props.user.administrator ?
                    <Form.Group className="mb-3">
                        <Form.Label>Author</Form.Label>
                        <Form.Select value={users.find((user) => user.id === authorId)?.email}
                                     onChange={handleUserChange}>
                            {users.map((user) => (
                                <option key={user.id} value={user.email}>
                                    {user.email}
                                </option>
                            ))}
                        </Form.Select>
                    </Form.Group> : null
                }


                <Form.Group className="mb-3">
                    <Form.Label>Publish Date</Form.Label>
                    <Form.Control type="date" name="date" value={publishDate} onChange={handleDate}/>
                </Form.Group>

                {blocks.map((block, index) => (
                    <BlockEditor
                        key={block.id}
                        blocks={blocks}
                        block={block}
                        setBlocks={setBlocks}
                        images={props.images}
                        onContentChange={handleBlockContent}
                        onOrderChange={handleBlockOrderChange}
                        onTypeChange={handleBlockTypeChange}
                    />
                ))}

                <Button variant="primary" onClick={() => setBlocks([...blocks, createBlock(blockType)])}>
                    Add Block
                </Button>
                {errorMsg ? (
                    <Alert variant="danger" onClose={() => setErrorMsg('')} dismissible>
                        {errorMsg}
                    </Alert>
                ) : null}

                <div className="d-flex justify-content-end mt-3">
                    <Button type="submit" variant="primary">
                        {objToEdit ? 'Save' : 'Add Page'}
                    </Button>
                    <Link to="/">
                        <Button className="mx-2" variant="danger">
                            Cancel
                        </Button>
                    </Link>
                </div>

            </Form>

            <div className="mt-4 p-2">
                <h3>{title}</h3>
                <p style={{color: 'gray'}}>(preview)</p>
                <Card>
                    <Card.Body>
                        {blocks.map((block) => (
                            <React.Fragment key={block.id}>{/*not add any additional elements into the DOM tree (warnings)*/}
                                {block.type !== 'image' ? (
                                    <>
                                        {block.type === 'header' && <h3>{block.content}</h3>}
                                        {block.type === 'paragraph' && <p>{block.content}</p>}
                                    </>
                                ) : (
                                    <img
                                        src={`${serverBaseUrl + block.imagePath}`}
                                        srcSet={`${serverBaseUrl + block.imagePath}`}
                                        alt="image"
                                        style={{ width: '250px', height: 'auto' }}
                                    />
                                )}
                            </React.Fragment>
                        ))}
                    </Card.Body>
                </Card>
            </div>
        </>
    );
}

function BlockEditor(props) {
    const {block, onContentChange, onOrderChange, onTypeChange, images} = props;
    const [selectedImage, setSelectedImage] = useState(images[0]);

    const handleContentChange = (ev) => {
        const content = ev.target.value;
        onContentChange(block.id, content);
    };

    const handleOrderIncrease = (orderIndex) => {
        const newOrder = orderIndex + 1;
        if (newOrder <= props.blocks.length) {
            onOrderChange(block.id, newOrder, 'increase');
        }
    };

    const handleOrderDecrease = (orderIndex) => {
        const newOrder = orderIndex - 1;
        if (newOrder >= 1) {
            onOrderChange(block.id, newOrder, 'decrease');
        }
    };

    const handleTypeChange = (ev) => {
        const type = ev.target.value;
        onTypeChange(block.id, type);
    };

    const handleBlockDelete = (blockId) => {
        const updatedBlocks = props.blocks
            .filter((block) => block.id !== blockId)
            .map((block, index) => ({...block, orderIndex: index + 1}));
        props.setBlocks(updatedBlocks);
    };

    const handleImageSelect = (image, block) => {
        setSelectedImage(image);
        onTypeChange(block.id, 'image', image);
    };

    return (
        <Card className="my-3">
            <Card.Header>
                <div className="d-flex justify-content-center align-items-center">


                    {block.orderIndex === props.blocks.length ? null : <Button
                        className='m-1'
                        variant="outline-primary"
                        onClick={() => handleOrderIncrease(block.orderIndex)}
                    >&#8595;</Button>}
                    {block.orderIndex === 1 ? null : <Button
                        className='m-1 m-lg-1'
                        variant="outline-primary"
                        onClick={() => handleOrderDecrease(block.orderIndex)}
                    >&#8593;</Button>}


                    <div className="d-flex align-items-center ms-5">
                        <Form.Select className="me-2 " style={{minWidth: '100%'}} value={block.type}
                                     onChange={handleTypeChange}>
                            {BLOCK_TYPES.map((type) => (
                                <option key={type} value={type}>
                                    {type}
                                </option>
                            ))}
                        </Form.Select>
                        <Button variant="danger" onClick={() => handleBlockDelete(block.id)}>
                            Delete
                        </Button>
                    </div>
                </div>
            </Card.Header>

            <Card.Body>
                {block.type !== 'image' ? (
                    <Form.Control
                        as={block.type === 'paragraph' ? 'textarea' : 'input'}
                        value={block.content}
                        onChange={handleContentChange}
                    />
                ) : (
                    <ImageList sx={{width: 800, height: 200}} cols={4} rowHeight={164}>
                        {images.map((image) => (
                            <ImageListItem
                                key={image.id}
                                onClick={() => handleImageSelect(image, block)}
                                sx={{cursor: 'pointer'}}
                            >
                                <img
                                    src={`${serverBaseUrl + image.imagePath}`}
                                    srcSet={`${serverBaseUrl + image.imagePath}`}
                                    alt="image"
                                    style={selectedImage?.id === image.id ? {border: '6px solid blue'} : {}}
                                />
                            </ImageListItem>
                        ))}
                    </ImageList>
                )}
            </Card.Body>
        </Card>
    );
}

export default FormRoute;
