from fastapi import FastAPI, APIRouter
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse

from app.database import SessionDep

from sqlmodel import select

from app.models.session import *
from app.models.reference import *
from app.models.derived import *


telemetry_router = APIRouter()


@telemetry_router.get("/sessions/{session_id}/telemetry")
async def get_telemetry(
    request: Request,
    session_id: int,
    db: SessionDep,
):
    response = db.get(ACSession, session_id)
    return {
        "status": "ok",
        "response": response
    }


@telemetry_router.post("/sessions/{session_id}/telemetry")
async def create_telemetry(
    request: Request,
    session_id: int,
    db: SessionDep,
):
    pass
