import {Alert, Container, Spinner} from 'react-bootstrap';
import * as React from 'react';
import {useState} from 'react';
import Card from '@mui/material/Card';
import CardActions from '@mui/material/CardActions';
import CardContent from '@mui/material/CardContent';
import CardMedia from '@mui/material/CardMedia';
import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';
import {IconButton} from "@mui/material";
import {Link} from 'react-router-dom';
import {MdAddCircleOutline, MdSwitchAccessShortcut} from "react-icons/md";
import dayjs from "dayjs";
import {FiEdit} from "react-icons/fi";


const serverBaseUrl = 'http://localhost:3001';

const buttonStyle = {
    backgroundColor: "transparent",
    color: "black",
    border: "none",
    padding: "10px",
    borderRadius: "0px",
    cursor: "pointer",
    fontSize: "28px",
    float: "right",
};

function MediaCard(props) {
    const {page} = props;

    return (
        <>
            <CardContent>
                <Typography variant="body2" color="text.secondary">
                    Page title: {page.title}
                </Typography>
            </CardContent>

            {page.blocks.map((block) => (
                <React.Fragment key={block.id}>
                    {block.type === 'image' ? (
                        <CardMedia sx={{height: 140}} image={serverBaseUrl + block.imagePath}/>
                    ) : null}
                    {block.type === 'header' ? (
                        <CardContent>
                            <Typography gutterBottom variant="h5" component="div">
                                {block.content}
                            </Typography>
                        </CardContent>
                    ) : null}
                    {block.type === 'paragraph' ? (
                        <CardContent>
                            <Typography variant="body2" color="text.secondary">
                                {block.content}
                            </Typography>
                        </CardContent>
                    ) : null}
                </React.Fragment>
            ))}


            {(props.login || props.admin) && props.backOffice ? <CardActions>
                <Link to={`/edit/${page.id}`}>
                    <Button size="small">Edit</Button>
                </Link>
                <Link to={`/delete/${page.id}/${page.title}`}>
                    <Button size="small">Delete</Button>
                </Link>
            </CardActions> : null}
            <CardContent>
                <Typography variant="body2" color="text.secondary">
                    CreationDate: {dayjs(page.creationDate).format('YYYY-MM-DD')}</Typography>
                <Typography variant="body2" color="text.secondary">
                    PublishDate: {page.publishDate? dayjs(page.publishDate).format('YYYY-MM-DD') : 'Draft'}
                </Typography>
                <Typography variant="body2" color="text.secondary">Author: {page.authorName}
                </Typography>
            </CardContent>
        </>
    )
        ;
}

function Loading() {
    return (
        <Spinner className='m-2' animation="border" role="status"/>
    )
}

function PageRoute(props) {

    const [backOffice, setBackOffice] = useState(false);

    return (
        <>
            <Container fluid>
                {props.loggedIn && (backOffice ? (
                    <button style={buttonStyle} onClick={() => setBackOffice(false)}>
                        <MdSwitchAccessShortcut />
                    </button>
                ) : (
                    <button style={buttonStyle} onClick={() => setBackOffice(true)}>
                        <FiEdit />
                    </button>
                ))}

                {props.errorMsg ? (
                    <Alert variant="danger" dismissible className="my-2" onClose={props.resetErrorMsg}>
                        {props.errorMsg}
                    </Alert>
                ) : null}
                {props.initialLoading ? (
                    <Loading/>
                ) : (
                    <>
                        <div style={{
                            display: 'flex',
                            flexWrap: 'wrap',
                            gap: '20px',
                            padding: '20px',
                            position: 'relative'
                        }}>
                            {props.loggedIn && backOffice ? (
                                props.pages.map((e) => (
                                    <Card key={e.id} sx={{width: '20%'}}
                                          style={{boxShadow: '1px 3px 4px rgba(0, 0, 0, 0.3)'}}>
                                        <MediaCard page={e} login={props.user && props.user.id === e.authorId}
                                                   admin={props.user && props.user.administrator}
                                                   backOffice={backOffice}/>
                                    </Card>
                                ))
                            ) : (
                                props.pubPages.map((e) => (
                                    <Card key={e.id} sx={{width: '20%'}}
                                          style={{boxShadow: '1px 3px 4px rgba(0, 0, 0, 0.3)'}}>
                                        <MediaCard page={e} login={props.user && props.user.id === e.authorId}
                                                   admin={props.user && props.user.administrator}
                                                   />
                                    </Card>
                                ))
                            )}

                            {props.user && backOffice ?
                                <Card style={{
                                    boxShadow: '1px 3px 4px rgba(0, 0, 0, 0.3)',
                                    display: 'flex',
                                    alignItems: 'center',
                                    justifyContent: 'center'
                                }} sx={{width: '20%'}}>
                                    <Link to="/add">
                                        <IconButton className="bg-white" style={{color: 'lightgrey'}}>
                                            <MdAddCircleOutline size="50%"/>
                                        </IconButton>
                                    </Link>

                                </Card> : null}
                        </div>

                    </>
                )}

            </Container>
        </>

    );
}

export default PageRoute;