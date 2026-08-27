from datetime import timezone
from fastapi import FastAPI, APIRouter
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse

from app.database import SessionDep

from sqlmodel import select

from app.models.session import *
from app.models.reference import *
from app.models.derived import *


session_router = APIRouter()


@session_router.get("/sessions")
async def get_session(
    request: Request,
    db: SessionDep,
):
    response = db.exec(select(ACSession)).all()
    return {
        "status": "ok",
        "response": response
    }


@session_router.get("/sessions/{session_id}")
async def get_session_by_id(
    request: Request,
    session_id: int,
    db: SessionDep,
):
    response = db.get(ACSession, session_id)
    return {
        "status": "ok",
        "response": response
    }


@session_router.post("/sessions")
async def create_session(
    request: Request,
    db: SessionDep,
):
    curr_session: ACSession = ACSession(
        session_type="...",
        started_at=datetime.now(timezone.utc),
        track_id=1,
        car_id=1
    )

    db.add(curr_session)
    await db.commit()
    await db.refresh(curr_session)
    return curr_session
