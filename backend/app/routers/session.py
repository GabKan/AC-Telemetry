from datetime import timezone
from fastapi import FastAPI, APIRouter
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse
from pydantic import BaseModel

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


class CreateSessionRequest(BaseModel):
    session_type: str
    track_id: int
    car_id: int
    started_at: datetime | None = None

class SessionResponse(BaseModel):
    id: int
    session_type: str
    started_at: datetime
    track_id: int
    car_id: int


@session_router.post("/sessions")
async def create_session(
    request_data: CreateSessionRequest,
    request: Request,
    db: SessionDep,
):
    """
    Create a new session.
    
    Expected JSON body:
    {
        "session_type": "practice",
        "track_id": 1,
        "car_id": 1,
        "started_at": "2025-08-28T10:30:00Z"
    }
    """
    started_at = request_data.started_at or datetime.now(timezone.utc)

    curr_session: ACSession = ACSession(
        session_type=request_data.session_type,
        started_at=started_at,
        track_id=request_data.track_id,
        car_id=request_data.car_id
    )

    db.add(curr_session)
    await db.commit()
    await db.refresh(curr_session)

    return curr_session
